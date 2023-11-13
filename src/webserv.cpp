/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgagnon <marvin@42quebec.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:08:16 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/09 11:08:17 by jgagnon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/library.hpp"
#include "../inc/conf.hpp"

void servers_routine(std::map<int, Conf *> *servers, char **env)
{
	struct timeval					refresh_time;
	fd_set							read_fds;
	fd_set 							write_fds;
	int								r_select;
	int								max_fd;
	std::vector<int>				*ready = new std::vector<int>;
	std::vector<int>				*client_fds = new std::vector<int>;
	std::string						response;

	refresh_time.tv_sec = 30;
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

		r_select = 0;
		response = "";

		FD_ZERO(&cpy_read);
		FD_ZERO(&cpy_write);
		FD_COPY(&read_fds, &cpy_read);
		FD_COPY(&write_fds,  &cpy_write);
		r_select = select(max_fd + 1, &cpy_read, &cpy_write, NULL, &refresh_time);
		if (r_select > 0)
		{
			for (std::map<int, Conf*>::iterator it_b = servers->begin() ; it_b != servers->end() ; it_b++)
			{
				int			client_socket;
				sockaddr_in	client_address;
				socklen_t	client_address_size = sizeof(client_address);
				
				client_socket = 0;
				if (FD_ISSET(it_b->first, &cpy_read) != 0)
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
				}
			}
			for (std::vector<int>::iterator it_beg = client_fds->begin(); it_beg != client_fds->end(); it_beg++)
			{
				if (FD_ISSET(*it_beg, &cpy_read) != 0)
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
						FD_CLR(*it_beg, &read_fds);
						FD_CLR(*it_beg, &cpy_read);
						FD_CLR(*it_beg, &write_fds);
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
				if (FD_ISSET(*it_beg, &cpy_write) != 0)
				{
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
							FD_CLR(*it_beg, &cpy_write);
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
		else if (r_select <= 0)
		{
			for (std::vector<int>::iterator it_beg = client_fds->begin(); it_beg != client_fds->end(); it_beg++) {
				FD_CLR(*it_beg, &write_fds);
				FD_CLR(*it_beg, &read_fds);
				FD_CLR(*it_beg, &cpy_write);
				FD_CLR(*it_beg, &cpy_read);
				close(*it_beg);
			}
			client_fds->clear();
			ready->clear();
			r_client.clear();
			delete client_fds;
			delete ready;
			for (std::map<int, Conf*>::iterator it_b = servers->begin() ; it_b != servers->end() ; it_b++)
				close(it_b->first);
			break ;
		}
	}
}

static bool SetsingleServer(std::string path, std::map<int, Conf*> *servers)
{
	Conf *single_serveur = new Conf();
	std::vector<std::string> *error_pages;

	error_pages = directory_parser("Error/");
	single_serveur->SetErrorPages(error_pages);
	single_serveur->SetPort(8000);
	single_serveur->SetBodySize(50000);
	single_serveur->SetAddrInfo();
	single_serveur->SetMethods("GET");
	single_serveur->SetServerContent(path);
	if (single_serveur->GetDirContent() == NULL || single_serveur->GetDirContent()->size() <= 0\
		|| count_extension(single_serveur->GetDirContent(), 0) <= 0)
	{
		clearservers(servers, single_serveur);
		std::cout << "Le chemin pour le serveur individuel contient aucun fichier" << std::endl;
		error_pages->clear();
		delete error_pages;
		return (false);
	}
	if (connectServer(servers, single_serveur) == false)
	{
		std::cout << "La connection pour le serveur individuel n'a pu etre etablie" << std::endl;
		clearservers(servers, single_serveur);	
		return (false);
	}
	return (true);
}

int main (int argc, char **argv, char **env)
{
	std::string				str_content;
	std::string				path;
	std::map<int, Conf*>	*servers = new std::map<int, Conf *>;
	int						serveur_count;
	bool					is_ok = false;

	if (argc != 2)
	{
		std::cout << "Veuillez fournir qu'un seul argument" << std::endl;
		return (1);
	}
	serveur_count = 0;
	servers->clear();
	path = argv[1];
	str_content = "";
	if (path.find(".conf") == std::string::npos)
	{
		if (access(argv[1], F_OK) != 0)
		{
			std::cout << "Le chemin ou le fichier pour le serveur individuel est inexistant ou inaccessible" << std::endl;
			delete servers;
			return (1);
		}
		is_ok = SetsingleServer(path, servers);
		if (is_ok == true)
			serveur_count = 1;
		else if (is_ok == false)
		{
			clearservers(servers, NULL);
			delete servers;
			return (1);
		}
	}
	if (servers->size() == 0)
		str_content = check_args(argc, argv, env);
	if (servers->size() == 0 && str_content.length() <= 0)
		return (1);
	if (serveur_count == 0 && std::count(str_content.begin(), str_content.end(), '{') == std::count(str_content.begin(), str_content.end(), '}') &&\
		std::count(str_content.begin(), str_content.end(), '{') == check_nbserver(str_content))
		serveur_count = std::count(str_content.begin(), str_content.end(), '{');
	if (serveur_count <= 0)
	{
		std::cout << "Il y a une erreur dans le formatage du fichier .conf" << std::endl;
		return (1);
	}
	if (servers->size() == 0)
		parse_file(str_content, serveur_count, servers, env);
	if (servers->size() > 0)
	{
		servers_routine(servers, env);
		clearservers(servers, NULL);
	}
	delete servers;
	return (0);
}
