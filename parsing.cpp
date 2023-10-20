#include "library.hpp"
#include "conf.hpp"

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

void parse_file(std::string content, int serveur_count, std::map<int, Conf *> servers)
{
	static int	nb_server;
	Conf		*conf_serv = new Conf();
	std::string	server_data;
	
	if (nb_server >= serveur_count || content.find("server") != 0 || content.find("server") == std::string::npos)
	{
		delete conf_serv;
		return ;
	}
	server_data = content.substr(content.find("{") + 1 , (content.find("}") - content.find("{") - 1));
	std::cout << "Je parse un serveur" << std::endl;
	std::cout << server_data;
	nb_server++;
	content = &content[content.find("}")];
	parse_file(content, serveur_count, servers);
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
	parse_file(str_content, serveur_count, servers);
}