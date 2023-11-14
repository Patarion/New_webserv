/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgagnon <marvin@42quebec.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:07:23 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/09 11:07:24 by jgagnon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/library.hpp"
#include "../inc/conf.hpp"

static const char *conf_var[6] = {"server_name", "client_body", "listen", "error_page", "allow_methods", NULL};

std::string check_args(int argc, char **argv, char **env)
{
	std::string file_name;
	if (!env)
	{
		std::cout << "Repartez le terminal puisque l'enironnement n'est pas disponible" << std::endl;
		return ("");
	}
	else if (argc != 2)
	{
		std::cout << "Veuillez fournir un seul argument et un fichier de configuration de format .conf" << std::endl;
		return ("");
	}
	file_name = argv[1];
	if (file_name.find(".conf") == std::string::npos && access(argv[1], F_OK) == 0)
		return (file_name);
	else if (std::count(file_name.begin(), file_name.end(), '.') != 1 || file_name.find(".conf") != (file_name.length() - 5))
	{
		std::cout << "Le type de fichier donné n'est pas valide. Veuillez fournir un fichier avec l'extension .conf" << std::endl;
		return ("");
	}
	std::ifstream file(file_name.c_str(), std::ios::in | std::ios::binary);
	if (!file)
	{
		std::cout << "Le fichier mis en paramètre n'a pu être ouvert" <<  std::endl;
		return ("");
	}
	std::ostringstream content;

	content << file.rdbuf();
	file.close();
	return(content.str());
}

static bool setServeurInfo(std::string line, Conf *serveur, int i)
{
	switch (i)
	{
		case 0:
		{
			serveur->SetServerName(line);
			return (true);
		}
		case 1:
		{
			long int body;
			try {
				body = std::stol(line);
			}
			catch (std::exception &e)
			{
				return (false);
			}
			if (body > 0)
			{
				serveur->SetBodySize(static_cast<int>(body));
				return (true);
			}
			return (false);
		}
		case 2:
		{
			long int port;

			port = 0;
			try{
				port = std::stol(line);
			}
			catch (std::exception &e)
			{
				return (false);
			}
			if (port > 0 && port != 80 && port <= 65353)
			{
				serveur->SetPort(static_cast<int>(port));
				return (true);
			}
			return (false);
		}
		case 3:
		{
			std::vector<std::string>	*dir_content;
			dir_content = directory_parser(line);
			if (dir_content->size() <= 0 || (count_extension(dir_content, 0) <= 0 && count_extension(dir_content, 4) <= 0))
			{
				delete dir_content;
				return (false);
			}
			serveur->SetErrorPages(dir_content);
			return (true);
		}
		case 4:
		{
			serveur->SetMethods(line);
			if (serveur->GetGet() == false)
				return (false);
			return (true);
		}
		default:
		{
			std::cout << "Si tu rentre ici, c'est comme rentrer dans un trou d'pet. T'es dans la marde" << std::endl;
			return (false);
		}
	}
}

static bool checkServer(Conf *server)
{
	if (server->GetPort() == 0 || server->GetBodySize() == 0 || server->GetGet() == false)
		return (false);
	return (true);
}

static bool setServeur(std::string data, Conf *serveur)
{
	std::string line;
	std::string	cpy_var_name;
	int			cycle;
	bool		treated[5] = { 0 };

	line = "";
	cpy_var_name = "";
	cycle = std::count(data.begin(), data.end(), '\n');
	while (cycle > 0)
	{
		for (int i = 0; conf_var[i] != NULL ; i++)
		{
			cpy_var_name = conf_var[i];
			if (data.find(cpy_var_name) != std::string::npos && data.find(cpy_var_name) == 0)
			{
				int	j;
				for (j = 0; conf_var[j] != NULL ; j++)
				{
					std::string	cpy_conf_var = conf_var[j];
					if (data.find(cpy_conf_var) != std::string::npos && std::strncmp(data.c_str(), cpy_conf_var.c_str(), cpy_conf_var.length()) == 0)
						break ;
				}
				if (treated[i] == false && j < 5)
				{
					line = &data[data.find(cpy_var_name) + cpy_var_name.length()];
					line = line.substr(0, line.find_first_of('\n'));
					treated[i] = true;
					if (setServeurInfo(line, serveur, i) == false)
						return (false);
				}
				break ;
			}
		}
		data = &data[data.find_first_of('\n') + 1];
		cycle--;
	}
	if (checkServer(serveur) == false)
		return (false);
	serveur->SetServerContent("Website/");
	return (true);
}

void parse_file(std::string content, int serveur_count, std::map<int, Conf *> *servers, char **env)
{
	static int	nb_server;
	std::string	server_data;

	if (nb_server >= serveur_count || content.find("server\n") == std::string::npos || servers == NULL)
		return ;
	server_data = content.substr(content.find("{") + 1 , (content.find("}") - content.find("{") - 1));
	server_data.erase(std::remove_if(server_data.begin(), server_data.end(), keepsomeSpace), server_data.end());
	if (server_data[0] == '\n')
		server_data = &server_data[1];

	Conf	*conf_serv = new Conf();
	if (conf_serv == NULL)
		return(clearservers(servers, conf_serv));
	if (setServeur(server_data, conf_serv) == false)
	{
		std::cout << "Une des infos dans le fichier de config est errone" << std::endl;
		return(clearservers(servers, conf_serv));
	}
	if (connectServer(servers, conf_serv) == false)
	{
		std::cout << "Au moins l'un des port du fichier de config n'est pas disponible" << std::endl;
		return(clearservers(servers, conf_serv));
	}
	nb_server++;
	content = &content[content.find("}") + 2];
	parse_file(content, serveur_count, servers, env);
}
