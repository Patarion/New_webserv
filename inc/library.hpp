/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgagnon <marvin@42quebec.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:08:45 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/09 11:08:47 by jgagnon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRARY_HPP
# define LIBRARY_HPP
# include <iostream>
# include <string>
# include <cstring>
# include <fstream>
# include <sstream>
# include <cstdio>
# include <cstdlib>
# include <sys/socket.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sys/poll.h>
# include <dirent.h>
# include <list>
# include <sys/time.h>
# include <sys/wait.h>
# include <sys/select.h>
# include <fcntl.h>
# include <vector>
# include <map>
# include <cctype>
# include <algorithm>
# include "../inc/conf.hpp"

# define MAX_BUFF_SIZE 2000000

class Conf;

std::vector<std::string>    *directory_parser(std::string dir);
std::string                 readfileContent(std::string path, char **env);
int                         extension_check(const char *path);
std::string					CGI_Handler(std::string file, char **env);
bool						keepsomeSpace(unsigned char c);
void						clearservers(std::map<int, Conf *> *servers, Conf *conf_serv);
bool						connectServer(std::map<int, Conf *> *servers, Conf *conf_serv);
std::string 				get_handler(std::vector<char> r_client, Conf *server, char **env);
std::string					post_handler(std::vector<char> r_client, Conf *server, int fd, int ret_recv, char **env);
std::string					error_handler(std::vector<std::string> *dir_content, std::string err_code, char **env);
std::string					treat_calculate(std::vector<std::string> *err_content,std::string data, char **env);
std::string					treat_post(std::vector<char> content, const char *delim, Conf *server, char **env);
std::string					post_response(int code, std::vector<std::string> *dir_content, char **env);
std::string					delete_handler(std::vector<char> r_client, Conf *server);
std::string					request_handler(const std::vector<char> r_client, Conf *server, char **env, int fd, int ret_recv);
std::string 				get_response_handler(std::string file, std::string file_content);
std::string					check_args(int argc, char **argv, char **env);
void						parse_file(std::string content, int serveur_count, std::map<int, Conf *> *servers, char **env);
std::string					treat_concours(std::vector<std::string> *err_content, std::string data, char **env);
unsigned long				checkallChar(std::string str);
int							check_nbserver(std::string file_content);
int							count_extension(std::vector<std::string> *dir_content, int extension);
void						Show_DirContent(std::vector<std::string> dir_content);
std::string					err_msg_gen(int err_code);

#endif
