/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dir_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gehebert <gehebert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 11:06:48 by jgagnon           #+#    #+#             */
/*   Updated: 2023/11/13 12:31:03 by gehebert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/library.hpp"

static const char *extensions_tab[9] = {".html", ".css", ".jpeg", ".png", ".php", ".gif", ".ico", ".js", NULL};

int extension_check(const char *path)
{
	int	j;
	std::string	str_path;
	std::string	str_extension;

	str_path = path;
	str_extension = "";
	j = 0;
	while (extensions_tab[j])
	{
		str_extension = extensions_tab[j];
		if (str_path.find(str_extension) != std::string::npos && str_path.find(str_extension) == (str_path.length() - str_extension.length()))
				return (j);
		j++;
	}
	return (j);
}

static void sub_directories_handler(DIR *dir, std::string path, std::vector<std::string> *path_list)
{
	struct dirent	*folder;
	DIR				*subdir;
	std::string		sub_path;

	folder = readdir(dir);
	sub_path = path;
	while (folder)
	{
		if (folder->d_name[0] != '.')
		{
			sub_path += folder->d_name;
			subdir = opendir(sub_path.c_str());
			if (subdir) 
			{
				sub_path += "/";
				sub_directories_handler(subdir, sub_path, path_list);
			}
			else if (extension_check(sub_path.c_str()) != 8)
				path_list->push_back(sub_path.c_str());
		}
		sub_path = path;
		folder = readdir(dir);
	}
	closedir(dir);
}

std::vector<std::string> *directory_parser(std::string dir)
{
	std::string 				path;
	std::vector<std::string>	*path_list;
	DIR	*directory;
	DIR	*tmpdir;
	struct dirent *folder;

	if (access(dir.c_str(), F_OK) != 0)
		return(NULL);
	if (dir[dir.length() - 1] != '/')
		dir += "/";
	directory = opendir(dir.c_str());
	path = "";
	path_list = new std::vector<std::string>;
	if (directory == NULL)
	{
		delete path_list;
		std::cout << "Le dossier source n'a pu être ouvert (CHMOD S'TI) ou est inexistant!" << std::endl;
		return (NULL);
	}
	folder = readdir(directory);
	while (folder)
	{
		if (folder->d_name[0] != '.')
		{
			path += dir + folder->d_name;
			tmpdir = opendir(path.c_str());
			if (tmpdir)
			{
				path += "/";
				sub_directories_handler(tmpdir, path, path_list);
			}
			else if (extension_check(folder->d_name) != 8)
				path_list->push_back(path);
		}
		path = "";
		folder = readdir(directory);
	}
//	Show_DirContent(*path_list);
	closedir(directory);
	return (path_list);
}
