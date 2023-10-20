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

static void Show_DirContent(std::vector<std::string> dir_content)
{
	std::vector<std::string>::iterator	it_b;
	std::vector<std::string>::iterator	it_e;

	it_b = dir_content.begin();
	it_e = dir_content.end();

	std::cout << "Voici le contenu du dossier" << std::endl;
	while (it_b != it_e)
	{
		std::cout << *it_b << std::endl;
		it_b++;
	}
}

//La fonction vérifie le contenu du dossier du server, si un p'tit malin fait un CHMOD du dossier c'est couvert :P
std::vector<std::string> *directory_parser(std::string dir)
{
	std::string 				path;
	std::vector<std::string>	*path_list;
	DIR	*directory;
	DIR	*tmpdir;
	struct dirent *folder;

	directory = opendir(dir.c_str()); // changer pour les dossiers émis dans le fichier de config
	path = "";
	path_list = new std::vector<std::string>;
	if (directory == NULL)
	{
		std::cerr << "Le dossier source n'a pu être ouvert. CHMOD S'TI" << std::endl;
		exit (EXIT_FAILURE);
	}
	folder = readdir(directory);
	// Une fois le dossier où on est situé est ouvert, on analyse où les fichiers utiles au site web sont situé pour conserver
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
    Show_DirContent(*path_list);
	closedir(directory);
	return (path_list);
}