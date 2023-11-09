/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgagnon <marvin@42quebec.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:06:59 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/09 11:07:00 by jgagnon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/library.hpp"
#include "../inc/conf.hpp"

static std::string err_msg_gen(int err_code)
{
	switch (err_code) {
		case 400 :
			return ("Content-Type: text/plain\r\nContent-Length: 11\r\n\r\nBad Request");
		case 403 :
			return ("Content-Type: text/plain\r\nContent-Length: 16\r\n\r\nAction Forbidden");
		case 404 : 
			return ("Content-Type: text/plain\r\nContent-Length: 19\r\n\r\nRessource not found");
		case 405 : 
			return ("Content-Type: text/plain\r\nContent-Length: 34\r\n\r\nMethod not allowed for this server");
		case 418 : 
			return ("Content-Type: text/plain\r\nContent-Length: 67\r\n\r\nTrying to divide by zero are yeh?! This won't make you coffee mate!");
		case 502 :
			return ("Content-Type: text/plain\r\nContent-Length: 42\r\n\r\nBad Gateway the client connection sharted!");
		case 507 : 
			return ("Content-Type: text/plain\r\nContent-Length: 35\r\n\r\nThe file is too big for this server");
		default :
			return ("Content-Type: text/plain\r\nContent-Length: 38\r\n\r\nFigure this one out. I won't help you!");
	}
}

static std::string err_header_gen(int err_code)
{
	switch (err_code) {
		case 400 :
			return ("HTTP/1.1 400 Bad Request\r\n"); 
		case 403 :
			return ("HTTP/1.1 403 Forbidden Action\r\n");
		case 404 :
			return ("HTTP/1.1 404 Not Found\r\n");
		case 405 :
			return ("HTTP/1.1 405 Method Not Allowed\r\n");
		case 418 :
			return ("HTTP/1.1 418 I'm a teapot\r\n");
		case 502 :
			return ("HTTP/1.1 502 Bad Gateway\r\n");
		case 507 :
			return ("HTTP/1.1 507 Insufficient Storage\r\n");
		default :
			return ("HTTP/1.1 500 Internal Server Error\r\n");
	}
}

std::string error_handler(std::vector<std::string> *dir_content, std::string err_code, char **env)
{
	std::vector<std::string>::iterator	it_b;
	std::vector<std::string>::iterator	it_e;
	std::ostringstream 					response;
	std::string 						r_file;
	std::string							path;

	if (dir_content == NULL || dir_content->size() == 0)
	{
		response << err_header_gen(std::stoi(err_code)) << err_msg_gen(std::stoi(err_code));
		return (response.str());
	}
	r_file = "";
	path = "";
	it_b = dir_content->begin();
	it_e = dir_content->end();
	while (it_b != it_e)
	{
		path = *it_b;
		if (path.find(err_code) != std::string::npos)
			break ;
		it_b++;
	}
	if (it_b != it_e)
	{
		r_file = readfileContent(path, env);
		response << err_header_gen(std::stoi(err_code)) << "Content-Type: text/html\r\nContent-Length: "<<\
		r_file.length() << "\r\n\r\n" << r_file;
	}
	else if (it_b == it_e)
		response << err_header_gen(std::stoi(err_code)) << err_msg_gen(std::stoi(err_code));
	return (response.str());
}
