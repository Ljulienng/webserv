#include "socket.hpp"

/* GETTERS */

int						&Socket::getFd()
{ return _fd; }

std::string				&Socket::getServerName()
{ return _serverName; }

struct sockaddr_in 		&Socket::getAddr()
{ return _addr; }


/* SETTERS */
void        Socket::setFd(int fd)
{
	_fd = fd;
}

void        Socket::setAddr(int family, const char *ip, unsigned short port)
{
	_addr.sin_family = family;
	_addr.sin_addr.s_addr = inet_addr(ip);
	_addr.sin_port = htons(port);
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Socket::Socket() :
			_fd(),
			_Addr(),
			_serverName()
			// to be completed if new attributes
{}

Socket::Socket(int sockFd, struct sockaddr_in sockaddr, std::string serverName) :
			_fd(sockFd),
			_Addr(sockaddr),
			_serverName(serverName)
			// to be completed if new attributes
{}

Socket::Socket(const Socket &src)
{
	*this = src;
}

Socket::~Socket() {}

Socket &Socket::operator=(const Socket &src)
{
	if (&src != this)
	{
	
		// to be completed if new attributes
	}
	return (*this);
}