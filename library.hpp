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

static const char *extensions_tab[9] = {".html", ".css", ".jpeg", ".png", ".php", ".gif", ".ico", ".js", NULL};

std::vector<std::string> *directory_parser(std::string dir);

#endif