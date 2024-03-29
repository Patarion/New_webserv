/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgagnon <marvin@42quebec.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:07:11 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/09 11:07:14 by jgagnon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/conf.hpp"
#include "../inc/library.hpp"

std::string 	get_handler(std::vector<char> r_client, Conf *server, char **env)
{
	std::vector<std::string>::iterator	it_b;
	std::vector<std::string>::iterator	it_e;
	std::ostringstream					stream_request;
	std::string							request;
	std::string							r_file;
	std::string							path;
	std::string							file_to_find;
	std::string							racine;
	std::string							roughPath;
	std::string							str_client;
	request = "";
	r_file = "";
	path = "";
	str_client = "";
	str_client.append(r_client.data());		
	it_b = server->GetDirContent()->begin();
	it_e = server->GetDirContent()->end();
	if (str_client.find("GET / HTTP/1.1") != std::string::npos)
		file_to_find = "index.html";
	else if (str_client.find("GET /") != std::string::npos)
		file_to_find = str_client.substr(4, str_client.find(" HTTP/1.1") - 4);
	while (it_b != it_e)
	{
		path = *it_b;
		if (path.find(file_to_find) != std::string::npos)
		{
			path = &path[path.find(file_to_find)];
			if (std::strncmp(path.c_str(), file_to_find.c_str(), file_to_find.length() + 1) == 0)
				break ;
		}
		it_b++;
	}
	if (it_b != it_e)
	{
		request += *it_b;
		if (extension_check(request.c_str()) == 4)
			r_file = CGI_Handler(request, env);
		else
			r_file = readfileContent(request, env);
		if (r_file.length() == 0)
		{
			std::cout << "Le fichier suivant : " << *it_b << " n'a pu être ouvert" << std::endl;
			return (error_handler(server->GetErrContent(), "500", env));
		}
		stream_request << get_response_handler(request, r_file);
	}
	else if (it_b == it_e)
		stream_request << error_handler(server->GetErrContent(), "404", env);
	return (stream_request.str());
}

std::string 	get_response_handler(std::string file, std::string file_content)
{
	std::ostringstream r_response;

	r_response << "HTTP/1.1 200 OK\r\n";
	switch (extension_check(file.c_str()))
	{
		case 0: {
			r_response << "Content-Type: text/html\r\nContent-Length: " << file_content.length() << "\r\n\r\n" << file_content;
			break ;
		}
		case 1: {
			r_response << "Content-Type : text/css\r\nContent-Length: " << file_content.length() << "\r\n\r\n" << file_content;
			break ;
		}
		case 2: {
			r_response << "Content-Type: image/jpeg\r\nContent-Length: " << file_content.length() << "\r\n\r\n" << file_content;
			break ;
		}
		case 3: {
			r_response << "Content-Type: image/png\r\nContent-Length: " << file_content.length() << "\r\n\r\n" << file_content;
			break ;
		}
		case 4: {
			r_response << "Content-Type: text/html\r\nContent-Length: " << file_content.length() << "\r\n\r\n" << file_content;
			break ;
		}
		case 5: {
			r_response << "Content-Type: image/gif\r\nContent-Length: " << file_content.length() << "\r\n\r\n" << file_content;
			break ;
		}
		case 6: {
			r_response << "Content-Type: image/x-icon\r\nContent-Length: " << file_content.length() << "\r\n\r\n" << file_content;
			break ;
		}
	}
	return (r_response.str());
}

static std::string calculate_response(std::string result)
{
	std::ostringstream	response;
	int					length;

	length = 179 + result.length();
	response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << length << "\r\n\r\n" <<\
		"<!DOCTYPE html><html><title>Le resultat de la calculatrice</title><body><h1>Le resultat du calcul est " <<\
		result << "</h1><br><a href=\"./index.html\">Retourner a la page initial</a></body></html>";
	return (response.str());
}

static std::string calculate_form(long double num1, long double num2, std::vector<std::string> *err_content, std::string data, char **env)
{
	std::ostringstream	str_stream;
	std::string 		result;

	result = "";
	if (data.find("&operator=add") != std::string::npos)
		num1 += num2;
	else if (data.find("&operator=substract") != std::string::npos)
		num1 -= num2;
	else if (data.find("&operator=multiply") != std::string::npos)
		num1 *= num2;
	else if (data.find("&operator=divide") != std::string::npos)
	{
		if (num2 == 0)
			return (error_handler(err_content, "418", env));
		num1 /= num2;
	}

	str_stream << num1;
	result = str_stream.str();
	return (calculate_response(result));
}

std::string treat_calculate(std::vector<std::string>  *err_content, std::string data, char **env)
{
	long double		num1;
	long double		num2;
	std::string		result;
	std::string		str_nbr;
	int				index;

	num1 = 0;
	num2 = 0;
	result = "";
	index = 0;
	str_nbr = "";
	
	str_nbr = data.substr(5, (data.find("&operator=") - 5));
	try {
		num1 = std::stold(str_nbr);
	}
	catch (std::invalid_argument)
	{
		return (error_handler(err_content, "400", env));
	}
	str_nbr = data.substr(data.find("&num2=") + 6, data.find("&submit=") - (data.find("&num2=") + 6));
	try {
		num2 = std::stold(str_nbr);
	}
	catch (std::invalid_argument)
	{
		return (error_handler(err_content, "400", env));
	}
	result = calculate_form(num1, num2, err_content, data, env);
	return(result);
}

std::string treat_concours(std::vector<std::string> *err_content, std::string data, char **env)
{
	std::string			value;
	std::string			r_file;
	std::ofstream		database;
	std::ostringstream	str_to_append;
	std::ostringstream	response;

	value = "";
	r_file = "";
	database.open("database.txt", std::ios::app);
	value = data.substr(4, (data.find("&prenom=") - 4));
	if (checkallChar(value) != value.length())
		return (error_handler(err_content, "400", env));
	str_to_append << "Participant" << std::endl;
	str_to_append << "Nom : " << value << std::endl;
	value = data.substr((data.find("&prenom=") + 8), (data.find("&submit=") - (data.find("&prenom=") + 8)));
	if (checkallChar(value) != value.length())
		return (error_handler(err_content, "400", env));
	str_to_append << "Prenom : " << value << std::endl;
	database << str_to_append.str();
	database.close();
	r_file = readfileContent("Website/form/treated.php", env);
	response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n" <<\
		"Content-Length: " << r_file.length() << "\r\n\r\n" << r_file;
	return (response.str());
}
