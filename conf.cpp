#include "library.hpp"
#include "conf.hpp"

Conf::Conf() : _methods() {
};

Conf::Conf(Conf &src)
{
	*this = src; //Copier les variables
};

Conf::~Conf() {};

void Conf::SetErrorPages(std::string dir) {
	_error_pages = directory_parser(dir);
};


void Conf::SetServerContent(std::string dir) {
	_html_content = directory_parser(dir);
};

void Conf::SetServerName(std::string name)  {
	_name = name;
};

void Conf::SetBodySize(int size) {
	_body = size;
};

Conf &Conf::operator=(Conf &){
	return (*this);
};

void Conf::SetAddress(int addr) {
	_address = addr;
};
void Conf::SetPort(int port) {
	_port = port;
};

void Conf::SetMethods(std::string methods) {
	if (methods.find("GET") != std::string::npos)
		_methods[0] = true;
	if (methods.find("POST") != std::string::npos)
		_methods[1] = true;
	if (methods.find("DELETE") != std::string::npos)
		_methods[2] = true;
}

	std::string					GetErrorPage(std::string page);
	std::string					GetDirContent(std::string page);
	int							GetPort();
	bool						GetMethod(std::string method);
	int							GetAddress();
	int							GetBodySize();