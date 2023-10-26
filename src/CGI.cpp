#include "../inc/library.hpp"
#include "../inc/conf.hpp"

std::string 	CGI_Handler(std::string file, char **env)
{
	std::string	r_CGI;

		int			i;
		char		char_file[512];
		pid_t		proc_exec;
		int			pipe_fd[2];
		char		buffer[1000000];
//		int			status;
		
	i = 0;
	if (pipe(pipe_fd) == -1)
	{
		//error handler here
		return (r_CGI);
	}
	while (file[i] != '\0')
	{
		char_file[i] = file[i];
		i++;
	}
	char_file[i] = '\0';
	char *const arg_tab[] = {(char *)"/usr/bin/php" , char_file, NULL};

	proc_exec = fork();
	if (proc_exec == -1)
	{
		// error handler here
		return (r_CGI);
	}
	else if (proc_exec == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		
		/*	spec execve ...*/
		if (execve("/usr/bin/php", arg_tab, env) == -1)
		{
			std::cout << "Exceve est de la marde" << std::endl;
			//error handler here
			exit(EXIT_FAILURE);
		}
	}
	else if (proc_exec > 0)
	{
		close(pipe_fd[1]);
		int		r_read;
		while ((r_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
			r_CGI.append(buffer, r_read);
	}
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	std::cout << "Exiting from CGI" << std::endl;
	std::cout << r_CGI << std::endl;

	return (r_CGI);
}

std::string request_handler(const std::vector<char> r_client, Conf *server, char **env, int fd, int ret_recv)
{
	std::string	r_request;
	std::string cpy_client;
	r_request = "";
	cpy_client = "";
	cpy_client.append(r_client.data());

	if (cpy_client.find("GET /") != std::string::npos)
		r_request = get_handler(r_client, server, env);
	else if (cpy_client.find("POST /") != std::string::npos && server->GetPost() == true)
		r_request = post_handler(r_client, server, fd, ret_recv, env);
	else if (cpy_client.find("DELETE /") != std::string::npos && server->GetDelete() == true)
		r_request = delete_handler(r_client, server);
	else if (cpy_client.find("POST /") != std::string::npos && server->GetPost() == false)
		r_request = error_handler(server->GetErrContent(), "403", env);
	else if (cpy_client.find("DELETE /") != std::string::npos && server->GetDelete() == false)
		r_request = error_handler(server->GetErrContent(), "403", env);
	return (r_request);
}

std::string delete_handler(std::vector<char> r_client, Conf *server)
{
	std::string							file_to_find;
	std::vector<std::string>::iterator	it_b;
	std::vector<std::string>::iterator	it_e;
	std::string 						str_client;
	std::string							r_handler;
	std::string							path;

	it_b = server->GetDirContent()->begin();
	it_e = server->GetDirContent()->end();
	str_client = r_client.data();
	file_to_find = str_client.substr(7, str_client.find(" HTTP/1.1") - 7);
	r_handler = "";
	while (it_b != it_e)
	{
		path = *it_b;
		if (path.find(file_to_find) != std::string::npos)
			break ;
		it_b++;
	}
	if (it_b != it_e)
	{
		server->GetDirContent()->erase(it_b);
		r_handler = "HTTP/1.1 204 No Content\r\n\r\n";
	}
	else if (it_b == it_e)
		r_handler = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 18\r\n\r\nResource not found";
	return (r_handler);
}
