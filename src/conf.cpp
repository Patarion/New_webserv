/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgagnon <marvin@42quebec.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:06:25 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/09 11:06:27 by jgagnon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/library.hpp"
#include "../inc/conf.hpp"

Conf::Conf() {
	_html_content = NULL;
	_error_pages = NULL;
	_handled_fds = NULL;
	_get = false;
	_post = false;
	_delete = false;
	_name = "";
	_body = 0;
	_port = 0;
	_address = 0;
	_err_path = "";
	_html_path = "";
	memset((void *) &_addr_data, 0, sizeof(_addr_data));
	_error_pages = NULL;
	_html_content = NULL;
	_handled_fds = NULL;

}

Conf::Conf(Conf &src)
{
	*this = src; //Copier les variables
}

Conf::~Conf() {
	if (_html_content != NULL && _html_content->size() > 0)
		_html_content->clear();
	if (_html_content != NULL)
		delete _html_content;
	if (_error_pages != NULL && _error_pages->size() > 0)
		_error_pages->clear();
	if (_error_pages != NULL)
		delete _error_pages;
	if (_handled_fds != NULL)
	{
		if (_handled_fds->size() > 0)
		{

			std::vector<int>::iterator	it_b = _handled_fds->begin();
			std::vector<int>::iterator	it_e = _handled_fds->end();
			while (it_b != it_e)
			{
				close(*it_b);
				it_b++;
			}
			_handled_fds->clear();
		}
		delete _handled_fds;
	}
}

void Conf::InitHandledFDs() {
	_handled_fds = new std::vector<int>;
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
	if (((methods.find("GETPOSTDELETE") != std::string::npos && methods.length() == 13) || (methods.find("GETPOST") != std::string::npos && methods.length() == 7)\
		|| (methods.find("GET") != std::string::npos && methods.length() == 3)))
	{
		if (methods.find("GET") != std::string::npos)
			_get = true;
		if (methods.find("POST") != std::string::npos)
			_post = true;
		if (methods.find("DELETE") != std::string::npos)
			_delete = true;
	}
}

void Conf::SetAddrInfo() {
	memset((void *) &_addr_data, 0, sizeof(_addr_data));
	_addr_data.sin_family = AF_INET;
	_addr_data.sin_addr.s_addr = htonl(2130706433); //127.0.0.1 addresse locale
	_addr_data.sin_port = htons(this->_port);

}

std::vector<std::string> *Conf::_CpyDirContent(std::vector<std::string> *src)  {
	std::vector<std::string>			*cpy = new std::vector<std::string>;
	std::vector<std::string>::iterator	it_b;

	if (src != NULL && src->size() > 0)
	{
		for (it_b = src->begin(); it_b != src->end(); it_b++)
			cpy->push_back(*it_b);
	}
	return (cpy);
}

std::vector<int> *Conf::_CpyHandledFDs(std::vector<int> *src){
	std::vector<int>				*cpy = new std::vector<int>;
	std::vector<int>::iterator		it_b;

	if (src != NULL && src->size() > 0)
		for (it_b = src->begin() ; it_b != src->end(); it_b++)
			cpy->push_back(*it_b);
	return (cpy);
}

void Conf::AddHandledFDs(int fd) {
	this->_handled_fds->push_back(fd);
}

bool Conf::CheckFD(int fd) {
	for (std::vector<int>::iterator it_b = this->_handled_fds->begin(); it_b != this->_handled_fds->end() ; it_b++)
		if (*it_b == fd)
			return (true);
	return (false);
}

void Conf::RemoveFD(int fd) {
	for (std::vector<int>::iterator it_b = this->_handled_fds->begin(); it_b != this->_handled_fds->end() ; it_b++)
	{
		if (*it_b == fd)
		{
			this->_handled_fds->erase(it_b);
			break ;
		}
	}

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
	if (this->_error_pages != NULL && this->_error_pages->size() > 0)
		delete this->_error_pages;
	if (this->_error_pages != NULL && this->_html_content->size() > 0)
		delete this->_html_content;
	if (this->_handled_fds != NULL && this->_handled_fds->size() > 0)
		delete this->_handled_fds;
	this->_error_pages = src._CpyDirContent(src.GetErrContent());
	this->_html_content = src._CpyDirContent(src.GetDirContent());
	this->_handled_fds = src._CpyHandledFDs(src.GetHandledFDs());
	return (*this);
}

std::string Conf::GetErrorPage(std::string page, char **env){
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
		page_content = readfileContent(*it_b, env);
	else if (it_b == it_e || page_content == "")
	{
		// Generate page
	}
	return (page_content);
}

std::string Conf::GetDirPage(std::string page, char **env){
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
		page_content = readfileContent(*it_b, env);
	else if (it_b == it_e && (extension_check(page.c_str()) == 0 || extension_check(page.c_str()) == 4))
		page_content = GetErrorPage("404", env);
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

std::vector<std::string>	*Conf::GetDirContent() {
	return (_html_content);
}

std::vector<std::string>	*Conf::GetErrContent() {
	return (_error_pages);
}

std::vector<int>			*Conf::GetHandledFDs() {
	return (_handled_fds);
}
