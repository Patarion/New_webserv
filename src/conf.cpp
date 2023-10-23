#include "../inc/library.hpp"
#include "../inc/conf.hpp"

Conf::Conf() {
	
}

Conf::Conf(Conf &src)
{
	*this = src; //Copier les variables
}

Conf::~Conf() {
	if (_html_content->size() > 0)
		delete _html_content;
	if (_error_pages->size() > 0)
		delete _error_pages;
}

void Conf::SetErrorPages(std::vector<std::string> *src) {
	_error_pages = src;
}

void Conf::SetServerContent(std::string dir) {
	_html_content = directory_parser(dir);
}

void Conf::SetServerName(std::string name)  {
	_name = name;
}

void Conf::SetBodySize(unsigned int size) {
	_body = size;
}


void Conf::SetAddress(unsigned int addr) {
	_address = addr;
}
void Conf::SetPort(unsigned int port) {
	_port = port;
}

void Conf::SetMethods(std::string methods) {
	if (methods.find("GET") != std::string::npos)
		_get = true;
	if (methods.find("POST") != std::string::npos)
		_post = true;
	if (methods.find("DELETE") != std::string::npos)
		_delete = true;
}

void Conf::SetAddrInfo() {
	memset((void *) &_addr_data, 0, sizeof(_addr_data));
	_addr_data.sin_family = AF_INET;
	_addr_data.sin_addr.s_addr = htonl(2130706433); //127.0.0.1 addresse locale
	_addr_data.sin_port = htons(this->_port);

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
	if (this->_error_pages->size() > 0)
		delete this->_error_pages;
	if (this->_html_content->size() > 0)
		delete this->_html_content;
	this->_error_pages = directory_parser(_err_path);
	this->_html_content = directory_parser(_html_path);
	return (*this);
}

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
		page_content = readfileContent(*it_b, this->_env);
	else if (it_b == it_e || page_content == "")
	{
		// Generate page
	}
	return (page_content);
}

std::string Conf::GetDirContent(std::string page){
	std::vector<std::string>::iterator	it_b;
	std::vector<std::string>::iterator	it_e;
	std::string							path;
	std::string							page_content;

	path = this->_html_path + page;
	it_b = this->_html_content->begin();
	it_e = this->_html_content->end();
	page_content = "";
	while (it_b != it_e)
	{
		if ((*it_b).find(path) != std::string::npos)
			break ;
		it_b++;
	}
	if (it_b != it_e)
		page_content = readfileContent(*it_b, this->_env);
	else if (it_b == it_e && (extension_check(page.c_str()) == 0 || extension_check(page.c_str()) == 4))
		page_content = GetErrorPage("404.html");
	return (page_content);
}

unsigned int Conf::GetPort() {
	return(_port);
}

unsigned int Conf::GetAddress() {
	return (_address);
}

unsigned int Conf::GetBodySize() {
	return (_body);
}

bool		Conf::GetGet() {
	return (_get);
}

bool		Conf::GetPost() {
	return (_post);
}

bool		Conf::GetDelete() {
	return (_delete);
}

std::string	Conf::GetServerName() {
	return (_name);
}

sockaddr_in	*Conf::GetAddrInfo() {
	return (&_addr_data);
}