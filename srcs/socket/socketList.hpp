#ifndef SOCKETLIST_HPP
# define SOCKETLIST_HPP

#include "webserv.hpp"
#include "socket.hpp"

class SocketList
{
    private :
		std::list<Socket*>		_sockets;

    public :
		SocketList();
		SocketList(const SocketList &src);
		~SocketList();		

		SocketList &operator=(const SocketList &src);

		void	addSocket(Socket* newSocket);

		// GETTERS
		std::list<Socket*>		getSockets();

};

#endif