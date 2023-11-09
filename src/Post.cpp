/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgagnon <marvin@42quebec.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:07:33 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/09 11:07:34 by jgagnon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/conf.hpp"
#include "../inc/library.hpp"

std::string double_quotes_trim(std::string r_client, int pos)
{
	std::string r_delim;
	int			end;

	end = 0;
	if (r_client[pos] != '"')
	{
		while (r_client[pos - 1] != '=')
			pos++;
		while (r_client[pos + end] != '\r' && r_client[pos + end] != '\n')
			end++;
	}
	else if (r_client[pos] == '"')
	{
		while (r_client[pos - 1] != '"')
			pos++;
		while(r_client[pos + end] != '"')
			end++;
	}
	end += pos;
	r_delim = r_client.substr(pos, (end - pos));
	return (r_delim);
}

static std::vector<char> check_transmission(std::vector<char> r_client, std::string delim, int fd, int ret_recv)
{
	std::string					cpy_delim;
	std::string					cpy_data;
	int							ret;
	std::vector<char>::iterator it_b;
	std::vector<char>::iterator it_e;
	std::vector<char>			data_recv(MAX_BUFF_SIZE);

	cpy_delim = "";
	cpy_data = "";
	cpy_delim += "--" + delim + "--";
	it_b = r_client.begin();
	it_e = r_client.end();
	while (it_b != it_e)
	{
		cpy_data += *it_b;
		it_b++;
	}
	while (cpy_data.find(cpy_delim) == std::string::npos && ret_recv < MAX_BUFF_SIZE)
	{
		ret = 0;
		data_recv.clear();
		data_recv.resize(MAX_BUFF_SIZE);
		ret = recv(fd, data_recv.data(), data_recv.size(), 0);
		data_recv.resize(ret);
		ret_recv += ret;
		it_b = data_recv.begin();
		it_e = data_recv.end();
		while (it_b != it_e)
		{
			r_client.push_back(*it_b);
			cpy_data += *it_b;
			it_b++;
		}
	}
	return (r_client);
}

std::string post_handler(std::vector<char> r_client, Conf *server, int fd, int ret_recv, char **env) // À voir, mais je risque d'avoir de besoin de l'URL de la page pour append les infos du site web
{
	std::ostringstream	r_post;
	std::string			data;
	std::string			str_client;
	std::string			delim;
	std::vector<char>::iterator it_b = r_client.begin();
	std::vector<char>::iterator it_e = r_client.end();
	int					pos;

	pos = 0;
	str_client = "";
	while (it_b != it_e)
	{
		str_client += *it_b;
		it_b++;
	}
	if (str_client.find("Content-Type: application/x-www-form-urlencoded\r\n") != std::string::npos)
	{
		pos = str_client.find("\r\n\r\n");
		data = str_client.substr(pos + 4, std::string::npos);
		if (data.find("num1=") != std::string::npos && data.find("&operator=") != std::string::npos &&\
			data.find("&num2=") != std::string::npos && data.find("&submit=Calculate") != std::string::npos)
			return (treat_calculate(server->GetErrContent(), data, env));
		else if (data.find("nom=") != std::string::npos && data.find("&prenom=") != std::string::npos &&\
			data.find("&submit=database") != std::string::npos)
			return (treat_concours(server->GetErrContent(), data, env));
	}
	else if (str_client.find("Content-Type: multipart/form-data;") != std::string::npos)
	{
		pos = str_client.find("Content-Type: multipart/form-data; boundary=");
		pos = str_client.find("boundary=") + 9;
		delim = double_quotes_trim(str_client, pos);
		r_client = check_transmission(r_client, delim, fd, ret_recv);
		if (r_client.size() >= MAX_BUFF_SIZE)
			return (post_response(507, server->GetErrContent(), env));
		data = treat_post(r_client, delim.c_str(), server, env);
	}
	return (data);
}

std::string	treat_post(std::vector<char> content, const char *delim, Conf *server, char **env)
{
	int	i;
	std::string data_name;
	std::string data;
	std::string file_name;
	std::string cpy_content;
	std::string cpy_delim;
	std::ofstream out_file;
	std::vector<char>::iterator it_b;
	std::vector<char>::iterator it_e;

	cpy_content = "";
	cpy_content.append(content.data());
	cpy_delim = delim;
	i = cpy_content.find(delim);
	while (cpy_content.find(delim) != std::string::npos)
	{
		file_name = "";
		data = "";
		data_name = "";
		if (cpy_content.find("Content-Disposition: form-data") != std::string::npos)
		{
			cpy_content = &cpy_content[cpy_content.find("Content-Disposition: form-data")];
			if ((cpy_content.find("filename") > cpy_content.find("\r\n")))
				break ;
			i = cpy_content.find("name=");
			cpy_content = &cpy_content[i + 5];
			data_name = double_quotes_trim(cpy_content.c_str(), 0);
			data += data_name + "&";
			i = cpy_content.find("filename=");
			cpy_content = &cpy_content[i + 9];
			file_name = double_quotes_trim(cpy_content.c_str(), 0);
			it_b = content.begin();
			it_e = content.end();
			cpy_content = "";
			cpy_content.append(content.data());
			i = cpy_content.find("\r\n\r\n") + 4;
			cpy_content = &cpy_content[i];
			i += cpy_content.find("\r\n\r\n") + 4;
			while (i > 0)
			{
				it_b++;
				i--;
			}
			cpy_content = "";
			while (it_b != it_e)
			{
				cpy_content += *it_b;
				it_b++;
				i++;
			}
			if (cpy_content.find(delim) != std::string::npos && \
				(cpy_content[cpy_content.find(delim) - 1] != '-' && cpy_content[cpy_content.find(delim) - 2] != '-'))
				data = cpy_content.substr(0, cpy_content.find(delim));
			else 
				data = cpy_content.substr(0, cpy_content.find(delim) - 2);
			cpy_content = &cpy_content[data.length()];
			out_file.open(file_name.c_str(), std::ios::out);
			if (!out_file.is_open())
			{
				std::cout << "Le fichier suivant : " << file_name << " n'a pu être crée" << std::endl;
				std::cout << "Veuillez retransférer le formulaire ou le fichier" << std::endl;
				return (post_response(500, server->GetErrContent(), env));
			}
			out_file << data;
			out_file.close();
		}
		i = cpy_content.find(delim);
		if (cpy_content[i + cpy_delim.length()] == '-' && cpy_content[i + 1 + cpy_delim.length()] == '-')
		{
			out_file.close();
			break ;
		}
	}
	return (post_response(200, server->GetDirContent(), env));
}


std::string	post_response(int code, std::vector<std::string> *dir_content, char **env)
{
	std::ostringstream	response;
	std::string			r_page;
	std::string			path;
	std::vector<std::string>::iterator it_b;
	std::vector<std::string>::iterator it_e;

	r_page = "";
	path = "";
	it_b = dir_content->begin();
	it_e = dir_content->end();
	if (code == 200)
	{
		while (it_b != it_e)
		{
			path = *it_b;
			if (path.find("Post_Success.html") != std::string::npos)
				break ;
			it_b++;
		}
		if (it_b == it_e)
			r_page = "<!DOCTYPE html><html><body>Transmission was a success</body></html>";
		else if (it_b != it_e)
			r_page = readfileContent(path, env);
		response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << r_page.length() << "\r\n\r\n" << r_page;
	}
	else if (code == 500)
		response << "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nInternal Server Error: Transmission failure\r\n";
	else if (code == 507)
		response << "HTTP/1.1 507 Insufficient Storage\r\n" << "Content-Type: text/plain\r\nContent-Length: 35\r\n\r\nThe file is too big for this server";
	return (response.str());
}
