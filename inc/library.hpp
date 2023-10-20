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
# include <vector>
# include <map>
# include <cctype>
# include <algorithm>

std::vector<std::string>    *directory_parser(std::string dir);
std::string                 readfileContent(std::string path, char **env);
int                         extension_check(const char *path);
std::string					CGI_Handler(std::string file, char **env);
bool						keepsomeSpace(unsigned char c);

#endif