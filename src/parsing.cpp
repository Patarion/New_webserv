#include "../inc/library.hpp"
#include "../inc/conf.hpp"

static const char *conf_var[6] = {"serveur_name", "client_body", "listen", "error_page", "allow_methods", NULL};

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
	if (std::count(file_name.begin(), file_name.end(), '.') != 1 || file_name.find(".conf") != (file_name.length() - 5))
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
	std::cout << "Information valide trouvée pour le serveur" << std::endl;
	switch (i)
	{
		case 0:
		{
//			serveur->SetServerName(line);
			return (true);
		}
		case 1:
		{	
			return (true);
		}
		case 2:
		{
			return (true);
			break ;
		}
		case 3:
		{
			return (true);
			break ;
		}
		case 4:
		{
			return (true);
		}
		default:
		{
			std::cout << "Si tu rentre ici, c'est comme rentrer dans un trou d'pet. T'es dans la marde" << std::endl;
			return (false);
		}
	}
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
	while (cycle >= 0)
	{
		for (int i = 0; conf_var[i] != NULL ; i++)
		{
			cpy_var_name = conf_var[i];
			if (data.find(cpy_var_name) != std::string::npos)
			{
				if (treated[i] == false)
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
		cycle--;
	}
	if (line.length() > 0)
		serveur->SetServerName(line);
	return (true);
}

	// server_name patate
	// client_body 100
	// listen localhost:8000
	// error_page html/error/
	// allow_methods GET POST DELETE

void parse_file(std::string content, int serveur_count, std::map<int, Conf *> servers, char **env)
{
	static int	nb_server;
	Conf		*conf_serv = new Conf();
	std::string	server_data;
	
	if (nb_server >= serveur_count || content.find("server") != 0 || content.find("server") == std::string::npos\
		|| content == "")
	{
		delete conf_serv;
		return ;
	}
	server_data = content.substr(content.find("{") + 1 , (content.find("}") - content.find("{") - 1));
	std::cout << "Je parse un serveur" << std::endl;
	std::cout << server_data;
	server_data.erase(std::remove_if(server_data.begin(), server_data.end(), keepsomeSpace), server_data.end());
	if (server_data[0] == '\n')
		server_data = &server_data[1];
	if (setServeur(server_data, conf_serv) == false)
	{
		delete conf_serv;
		std::map<int, Conf *>::iterator it_b;
		std::map<int, Conf *>::iterator it_e;

		it_b = servers.begin();
		it_e = servers.end();
		while (it_b != it_e)
		{
			delete it_b->second;
			it_b++;
		}
		return ;
	}
	nb_server++;
	content = &content[content.find("}") + 2];
	parse_file(content, serveur_count, servers, env);
}

int main (int argc, char **argv, char **env)
{
	std::string				str_content;
	std::map<int, Conf*>	servers;
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
}