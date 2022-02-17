#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "webserv.hpp"

class Socket
{
    public :

        size_t          _index; // to access in different vectors in hub.cpp

        enum    e_type
        {
            none,
            server,
            client,
            cgiFrom,
            cgiTo
        };

    protected :
        struct pollfd   _pollFd;
        sockaddr_in     _addr;
        std::string     _serverName;
        e_type          _type;

        Socket();

    public :
		
        // Socket(int fd, struct sockaddr_in addr, e_type type);
		Socket(const Socket &src);
		virtual ~Socket();		
		Socket &operator=(const Socket &src);

        // SETTERS
        void        setFd(int fd);
        void        setPollFd(struct pollfd pollFD);
        void        setAddr(sockaddr_in addr);
        void        setAddr(int family, const char *ip, unsigned short port);
        void        setServerName(std::string serverName);
        void        setType(e_type type);

        // GETTERS
        int						&getFd();
        struct pollfd		    &getPollFd();
		struct sockaddr_in 		&getAddr();
        std::string 		    &getServerName();
        e_type             		&getType();
		
};

#endif