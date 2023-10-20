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