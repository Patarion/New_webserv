#include "../inc/library.hpp"
#include "../inc/conf.hpp"

std::string readfileContent(std::string path, char **env) {
	if (extension_check(path.c_str()) == 4)
		return (CGI_Handler(path, env));
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file){
		std::cerr << "Le fichier mis en paramètre n'a pu être ouvert" << std::endl;
		return ("");
	}
	std::ostringstream content(std::ios::binary);

	content << file.rdbuf();
	file.close();
	return (content.str());
}

bool keepsomeSpace(unsigned char c) {
	return (c == '\r' || c == '\t' || c == '\v' || c == '\f' || c == 0x20);
}

void clearservers(std::map<int, Conf *> *servers, Conf *conf_serv) {

	if (conf_serv != NULL)
		delete conf_serv;
	std::map<int, Conf *>::iterator it_b;
	std::map<int, Conf *>::iterator it_e;
	
	if (servers->size() > 0)
	{
		it_b = servers->begin();
		it_e = servers->end();
		while (it_b != it_e)
		{
			close(it_b->first);
			delete it_b->second;
			it_b++;
		}
	}
	servers->clear();
	servers = NULL;
	return ;
}

unsigned long checkallChar(std::string str)
{
	int	nbr_char;

	nbr_char = 0;
	for (std::string::iterator it_b = str.begin() ; it_b != str.end() ; it_b ++)
	{
		if (std::isalpha(*it_b) != 0)
			nbr_char++;
	}
	return (nbr_char);
}

int	check_nbserver(std::string file_content)
{
	int	count;

	count = 0;
	while (1)
	{
		if (file_content.find("server\n") != std::string::npos)
		{
			count++;
			file_content = &file_content[file_content.find("server\n") + 8];
		}
		else if (file_content.find("server\n") == std::string::npos)
			break ;
	}
	return (count);
}

int	count_extension(std::vector<std::string> *dir_content, int extension)
{
	std::vector<std::string>::iterator	it_b;
	std::vector<std::string>::iterator	it_e;
	int									count;

	count = 0;
	it_b = dir_content->begin();
	it_e = dir_content->end();
	while (it_b != it_e)
	{
		if (extension_check((*it_b).c_str()) == extension)
			count++;
		it_b++;
	}
	return (count);
}

void Show_DirContent(std::vector<std::string> dir_content)
{
	std::vector<std::string>::iterator	it_b;
	std::vector<std::string>::iterator	it_e;

	it_b = dir_content.begin();
	it_e = dir_content.end();

	std::cout << "Voici le contenu du dossier" << std::endl;
	while (it_b != it_e)
	{
		std::cout << *it_b << std::endl;
		it_b++;
	}
}