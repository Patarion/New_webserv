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
	void						SetBodySize(unsigned int size);
	void						SetAddress(unsigned int addr);
	void						SetPort(unsigned int port);
	void						SetMethods(std::string methods);
	std::string					GetErrorPage(std::string page);
	std::string					GetDirContent(std::string file);
	unsigned int				GetPort();
	bool						GetMethod(std::string method);
	unsigned int				GetAddress();
	unsigned int				GetBodySize();
	short int					GetMethods();

	private :
	std::string					_name;
	unsigned int				_body;
	unsigned int				_port;
	unsigned int				_address;
	bool						_get;
	bool						_post;
	bool						_delete;
	std::string					_err_path;
	std::string					_html_path;
	std::vector<std::string>	*_error_pages;
	std::vector<std::string>	*_html_content;

};

#endif