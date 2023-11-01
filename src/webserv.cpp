#include "../inc/library.hpp"
#include "../inc/conf.hpp"

void servers_routine(std::map<int, Conf *> *servers, char **env)
{
	struct timeval					refresh_time;
	fd_set							read_fds;
	fd_set 							write_fds;
	int								cycle;
	int								r_select;
	int								max_fd;
	std::vector<int>				*ready = new std::vector<int>;
	std::vector<int>				*client_fds = new std::vector<int>;
	std::string						response;

	refresh_time.tv_sec = 10;
	refresh_time.tv_usec = 0;
	max_fd = 0;
	FD_ZERO(&write_fds);
	FD_ZERO(&read_fds);
	for (std::map<int, Conf*>::iterator it_b = servers->begin() ; it_b != servers->end() ; it_b++)
	{
		FD_SET(it_b->first, &read_fds);
		FD_SET(it_b->first, &write_fds);
		ready->push_back(it_b->first);
		if (max_fd < it_b->first)
			max_fd = it_b->first;
	}
	while (1)
	{
		static std::vector<char>	r_client(MAX_BUFF_SIZE);
		fd_set						cpy_read;
		fd_set						cpy_write;
		static int					r_recv;

		cycle = 2;
		r_select = 0;
		response = "";

		FD_ZERO(&cpy_read);
		FD_ZERO(&cpy_write);
		FD_COPY(&read_fds, &cpy_read);
		FD_COPY(&write_fds,  &cpy_write);
		while (r_select == 0)
		{
			r_select = select(max_fd + 1, &cpy_read, &cpy_write, NULL, &refresh_time);
			if (r_select == 0 && cycle > 0)
				cycle--;
			else if (r_select < 0 || (r_select == 0 && cycle <= 0))
				break ;
		}
		std::cout << "____ START r_select == " << r_select << " _______max_fd:" << max_fd << std::endl;
		if (r_select > 0)
		{
			for (std::map<int, Conf*>::iterator it_b = servers->begin() ; it_b != servers->end() ; it_b++)
			{
				int			client_socket;
				sockaddr_in	client_address;
				socklen_t	client_address_size = sizeof(client_address);
				
				client_socket = 0;
				if (FD_ISSET(it_b->first, &cpy_read))
				{
					client_socket = accept(it_b->first, (struct sockaddr *)&client_address, &client_address_size);
					if (client_socket != -1)
					{
						fcntl(client_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
						FD_SET(client_socket, &read_fds);
						client_fds->push_back(client_socket);
						it_b->second->AddHandledFDs(client_socket);
						if (client_socket > max_fd)
							max_fd = client_socket;
					}
					break ;
				}
			}
			for (std::vector<int>::iterator it_beg = client_fds->begin(); it_beg != client_fds->end(); it_beg++)
			{
				if (FD_ISSET(*it_beg, &cpy_read) > 0)
				{
					r_recv = recv(*it_beg, &r_client[0], r_client.size(), 0);
					if (r_recv > 0 && FD_ISSET(*it_beg, &write_fds) == 0)
					{
						ready->push_back(*it_beg);
						FD_SET(*it_beg, &write_fds);
						FD_SET(*it_beg, &cpy_write);
					}
					else if (r_recv < 0)
					{
						// FD_CLR(*it_beg, &read_fds);
						FD_CLR(*it_beg, &write_fds);
						FD_CLR(*it_beg, &cpy_write);
						FD_CLR(*it_beg, &cpy_write);
						client_fds->erase(it_beg);
						for (std::vector<int>::iterator it = ready->begin(); it != ready->end() ; it++)
							if (*it_beg == *it)
							{
								ready->erase(it);
								break ;
							}
						for (std::map<int, Conf*>::iterator it_b = servers->begin() ; it_b != servers->end() ; it_b++)
							if (it_b->second->CheckFD(*it_beg) == true)
								it_b->second->RemoveFD(*it_beg);
						close(*it_beg);
						break ;
					}
					if (r_recv > 0)
						r_client.resize(r_recv);
				}
			}
			for(std::vector<int>::iterator it_beg = ready->begin(); it_beg != ready->end(); it_beg++)
			{
				std::cout << "### FOR LOOP VECTOR ITER " << *it_beg <<  std::endl;
				if (FD_ISSET(*it_beg, &cpy_write))
				{
					std::cout << "### IF _ FD_ISSET succeded ### " <<  std::endl;
					for (std::map<int, Conf*>::iterator it_b = servers->begin() ; it_b != servers->end() ; it_b++)
					{
						if (it_b->second->CheckFD(*it_beg) == true)
						{
							response = request_handler(r_client, it_b->second, env, *it_beg, r_recv);
							if (response.length() > 0)
							{
								std::ofstream	tmp_outresponse("tmp.txt", std::ios::out);
								std::ifstream	tmp_inresponse;
								char			buffer[2048] = { 0 };
								int				read_bytes;

								read_bytes = 0;
								tmp_outresponse << response;
								tmp_outresponse.close();
								tmp_inresponse.open("tmp.txt");
								while ((read_bytes = tmp_inresponse.read(buffer, sizeof(buffer)).gcount()) > 0)
									while (send(*it_beg, buffer, read_bytes, 0) < 0);
								tmp_inresponse.close();
								std::remove("tmp.txt");
							}
							FD_CLR(*it_beg, &write_fds);
							// FD_CLR(*it_beg, &read_fds);
							FD_CLR(*it_beg, &cpy_write);
							// FD_CLR(*it_beg, &cpy_read);
							ready->erase(it_beg);
							r_client.clear();
							r_client.resize(MAX_BUFF_SIZE);
							response = "";
							r_recv = 0;
							break ;
						}
					}
					break ;
				}
			}
		}
		else if (r_select < 0 || (r_select == 0 && cycle <= 0))
		{
			if ( cycle <= 0)
				std::cout << "____ cycle EXIT_______" << std::endl;
			for (std::vector<int>::iterator it_beg = client_fds->begin(); it_beg != client_fds->end(); it_beg++) {
				std::cout << " closing Vect shit... *it_beg :: " << *it_beg << " :: _______ " << std::endl;
				FD_CLR(*it_beg, &write_fds);
				FD_CLR(*it_beg, &read_fds);
				FD_CLR(*it_beg, &cpy_write);
				FD_CLR(*it_beg, &cpy_read);
				close(*it_beg);
			}
			client_fds->clear();
			ready->clear();
			delete client_fds;
			delete ready;
			for (std::map<int, Conf*>::iterator it_b = servers->begin() ; it_b != servers->end() ; it_b++)
				close(it_b->first);
			}
			break ;
		}
		std::cout << "____ ENFIN CALISS... r_select == [" << r_select << "] " << std::endl;
	}
}

int main (int argc, char **argv, char **env)
{
	std::string				str_content;
	std::map<int, Conf*>	*servers = new std::map<int, Conf *>;
	int						serveur_count;

	str_content = check_args(argc, argv, env);
	serveur_count = 0;
	servers->clear();
	if (str_content.length() <= 0)
		exit (EXIT_FAILURE);
	if (std::count(str_content.begin(), str_content.end(), '{') == std::count(str_content.begin(), str_content.end(), '}'))
		serveur_count = std::count(str_content.begin(), str_content.end(), '{');
	if (serveur_count <= 0)
		exit (EXIT_FAILURE);
	parse_file(str_content, serveur_count, servers, env);
	if (servers->size() > 0)
	{
		servers_routine(servers, env);
		clearservers(servers, NULL);
	}
	else if (servers->size() == 0)
		std::cout << "Une erreur est survenue lors du parsing" << std::endl;
	delete servers;
}
