#ifndef LIBRARY_HPP
# define LIBRARY_HPP
# include <iostream>
# include <fstream>
# include <string>
# include <sstream>
# include <map>
# include <vector>
# include <sys/types.h>
# include <dirent.h>
# include <algorithm>

std::vector<std::string>    *directory_parser(std::string dir);
std::string                 readfileContent(std::string path);
int                         extension_check(const char *path);

#endif