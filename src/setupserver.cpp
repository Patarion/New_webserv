#include "../inc/conf.hpp"
#include "../inc/library.hpp"

bool connectServer(std::map<int, Conf*> *servers, Conf *serv) {
	int fd_serv;

	fd_serv = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_serv < 0)
		return (false);
	fcntl(fd_serv, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	serv->SetAddrInfo();
	if (bind(fd_serv, (struct sockaddr *)serv->GetAddrInfo(), sizeof(sockaddr_in)) == -1)
	{
		close (fd_serv);
		return (false);
	}
	if (listen(fd_serv, serv->GetBodySize()) < 0)
	{
		close(fd_serv);
		return (false);
	}
	servers->insert(std::make_pair(fd_serv, serv));
	return (true);
}