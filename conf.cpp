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


void Conf::SetAddress(int addr) {
	_address = addr;
};
void Conf::SetPort(int port) {
	_port = port;
};

void Conf::SetMethods(std::string methods) {
	if (methods.find("GET") != std::string::npos)
		_get = true;
	if (methods.find("POST") != std::string::npos)
		_post = true;
	if (methods.find("DELETE") != std::string::npos)
		_delete = true;
}

short int Conf::GetMethods() {
	short int methods;

	methods = 0;
	methods |= _get << 2;
	methods |= _post << 1;
	methods |= _delete;
	return (methods);
}

Conf &Conf::operator=(Conf &src){
	this->_name = src._name;
	this->_body = src._body;
	this->_port = src._port;
	this->_address = src._address;
	this->_get = src._get;
	this->_post = src._post;
	this->_delete = src._delete;
	this->_err_path = src._err_path;
	this->_html_path = src._html_path;
	this->_error_pages = directory_parser(_err_path);
	this->_html_content = directory_parser(_html_path);
	return (*this);
};

std::string Conf::GetErrorPage(std::string page){
	std::vector<std::string>::iterator	it_b;
	std::vector<std::string>::iterator	it_e;
	std::string							path;
	std::string							page_content;

	path = this->_err_path + page;
	it_b = this->_error_pages->begin();
	it_e = this->_error_pages->end();
	page_content = "";
	while (it_b != it_e)
	{
		if ((*it_b).find(path) != std::string::npos)
			break ;
		it_b++;
	}
	if (it_b != it_e)
		page_content = readfileContent(*it_b);
	else if (it_b == it_e || page_content == "")
	{
		// Generate page
	}
	return (page_content);
};

	std::string					GetErrorPage(std::string page);
	std::string					GetDirContent(std::string page);
	int							GetPort();
	bool						GetMethod(std::string method);
	int							GetAddress();
	int							GetBodySize();