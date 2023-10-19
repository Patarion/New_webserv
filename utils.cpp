#include "library.hpp"

std::string readfileContent(std::string path) {
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file){
		std::cerr << "readFileContent::Le fichier d'index n'a pu être ouvert" << std::endl;
		return ("");
	}
	std::ostringstream content;

	content << file.rdbuf();
	file.close();
	return (content.str());
}
