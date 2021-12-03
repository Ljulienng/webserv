#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "webserv.hpp"

class Socket
{
    protected :
        int             _fd;;
        sockaddr_in     _addr;

    public :
		Socket();
        Socket(int fd, struct sockaddr_in addr);
		Socket(const Socket &src);
		virtual ~Socket();		
		Socket &operator=(const Socket &src);

        // SETTERS
        void        setFd(int fd);
        void        setAddr(int family, const char *ip, unsigned short port);

        // GETTERS
        int						&getFd();
		struct sockaddr_in 		&getAddr();

		
};

#endif