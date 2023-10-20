#include "../inc/library.hpp"

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