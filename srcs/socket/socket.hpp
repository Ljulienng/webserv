#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "webserv.hpp"

class Socket
{
    private :
        int             _fd;;
        sockaddr_in     _addr;
        socklen_t       _addrLen;
        std::string     _serverName;

    public :
		Socket();
        Socket(int sockFd, struct sockaddr_in sockaddr, std::string serverName);
		Socket(const Socket &src);
		~Socket();		

		Socket &operator=(const Socket &src);

        // GETTERS
        int						&getFd();
        std::string				&getServerName();
		struct sockaddr_in 		&getAddr();

        void        setFd(int fd);
        void        setAddr(int family, const char *ip, unsigned short port);
		
};

#endif