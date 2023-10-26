#include "../inc/library.hpp"
#include "../inc/conf.hpp"

static std::string error_gen(int err_code)
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
		response << error_gen(std::stoi(err_code));
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
		response << error_gen(std::stoi(err_code)) << "Content-Type: text/html\r\nContent-Length: "<<\
		r_file.length() << "\r\n\r\n" << r_file;
	}
	else if (it_b == it_e)
		response << error_gen(std::stoi(err_code));
	return (response.str());
}
