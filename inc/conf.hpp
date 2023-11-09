/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgagnon <marvin@42quebec.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:08:33 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/09 11:08:34 by jgagnon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
# define CONF_HPP

# include "library.hpp"

class Conf {

	public :
	Conf();
	Conf(Conf &src);
	~Conf();
	Conf &operator=(Conf &);
	void						SetErrorPages(std::vector<std::string> *src);
	void						SetServerContent(std::string dir);
	void						SetServerName(std::string name);
	void						SetBodySize(unsigned int size);
	void						SetAddress(unsigned int addr);
	void						SetPort(unsigned int port);
	void						SetMethods(std::string methods);
	void						SetAddrInfo();
	void						InitHandledFDs();
	void						AddHandledFDs(int fd);
	std::string					GetErrorPage(std::string page, char **env);
	std::string					GetDirPage(std::string file, char **env);
	unsigned int				GetPort();
	unsigned int				GetAddress();
	unsigned int				GetBodySize();
	bool						GetGet();
	bool						GetPost();
	bool						GetDelete();
	char						**GetEnv();
	std::string					GetServerName();
	sockaddr_in					*GetAddrInfo();
	std::vector<std::string>	*GetDirContent();
	std::vector<std::string>	*GetErrContent();
	std::vector<int>			*GetHandledFDs();
	bool						CheckFD(int fd);
	void						RemoveFD(int fd);

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
	std::vector<int>			*_handled_fds;
	struct sockaddr_in			_addr_data;

	std::vector<std::string>	*_CpyDirContent(std::vector<std::string> *src);
	std::vector<int>			*_CpyHandledFDs(std::vector<int> *src);
};

#endif
