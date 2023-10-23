#include "../inc/library.hpp"
#include "../inc/conf.hpp"

std::string readfileContent(std::string path, char **env) {
	if (extension_check(path.c_str()) == 4)
		return (CGI_Handler(path, env));
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file){
		std::cerr << "Le fichier mis en paramètre n'a pu être ouvert" << std::endl;
		return ("");
	}
	std::ostringstream content;

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
	
	it_b = servers->begin();
	it_e = servers->end();
	while (it_b != it_e)
	{
		delete it_b->second;
		it_b++;
	}
	delete servers;
	return ;
}