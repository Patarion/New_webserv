#include "../inc/library.hpp"
#include "../inc/conf.hpp"

void servers_routine(std::map<int, Conf *> *servers, char **env)
{
	struct timeval	refresh_time;
	fd_set			base_fds;
	fd_set			read_fds;
	int				cycle;
	int				r_select;
	int				max_fd;
	std::map<int, Conf*>::iterator it_b = servers->begin();
	std::map<int, Conf*>::iterator it_e = servers->end();

	refresh_time.tv_sec = 10;
	refresh_time.tv_usec = 0;
	max_fd = 0;
	FD_ZERO(&base_fds);
	while (it_b != it_e)
	{
		FD_SET(it_b->first, &base_fds);
		if (max_fd < it_b->first)
			max_fd = it_b->first;
		it_b++;
	}
	while (1)
	{
		std::vector<char> r_client(MAX_BUFF_SIZE);
		cycle = 3;
		r_select = 0;
		while (r_select == 0)
		{
			FD_ZERO(&read_fds);
			FD_COPY(&base_fds, &read_fds);
			r_select = select(max_fd + 1, &read_fds, NULL, NULL, &refresh_time);
			if (r_select == 0 && cycle-- > 0);
			else if (r_select < 0 || (r_select == 0 && cycle <= 0))
				break ;
		}
		if (r_select > 0)
		{
			
		}
	}
}

int main (int argc, char **argv, char **env)
{
	std::string				str_content;
	std::map<int, Conf*>	*servers = new std::map<int, Conf *>;
	int						serveur_count;

	str_content = check_args(argc, argv, env);
	serveur_count = 0;
	if (str_content.length() <= 0)
		exit (EXIT_FAILURE);
	if (std::count(str_content.begin(), str_content.end(), '{') == std::count(str_content.begin(), str_content.end(), '}'))
		serveur_count = std::count(str_content.begin(), str_content.end(), '{');
	if (serveur_count <= 0)
		exit (EXIT_FAILURE);
	parse_file(str_content, serveur_count, servers, env);
	servers_routine(servers, env);
	clearservers(servers, NULL);
}
