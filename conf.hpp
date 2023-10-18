#ifndef CONF_HPP
# define CONF_HPP

# include "library.hpp"

class Conf {

	public :
	Conf();
	Conf(Conf &src);
	~Conf();
	Conf &operator=(Conf &);
	void						SetErrorPages(std::string dir);
	void						SetServerContent(std::string dir);
	void						SetServerName(std::string name);
	void						SetBodySize(int size);
	void						SetAddress(int addr);
	void						SetPort(int port);
	void						SetMethods(std::string methods);
	std::string					GetErrorPage(std::string page);
	std::string					GetDirContent(std::string file);
	int							GetPort();
	bool						GetMethod(std::string method);
	int							GetAddress();
	int							GetBodySize();

	private :
	std::string					_name;
	unsigned int				_body;
	int							_port;
	int							_address;
	bool						_methods[3];
	std::vector<std::string>	*_error_pages;
	std::vector<std::string>	*_html_content;

};

#endif