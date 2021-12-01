#ifndef SOCKETLIST_HPP
# define SOCKETLIST_HPP

#include "webserv.hpp"

class SocketList
{
    private :


    public :
		SocketList();
		SocketList(const SocketList &src);
		~SocketList();		

		SocketList &operator=(const SocketList &src);

		void	addSocket(int sockFd, struct sockaddr_in sockaddr, std::string serverName);

};

#endif