#include "socket.hpp"


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

/* GETTERS */
int						&Socket::getFd()
{
	return _fd;
}

struct sockaddr_in 		&Socket::getAddr()
{
	return _addr;
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Socket::Socket() :
			_fd(),
			_addr()
			// to be completed if new attributes
{}

Socket::Socket(int sockFd, struct sockaddr_in sockaddr) :
			_fd(sockFd),
			_addr(sockaddr)
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
		_fd = src._fd;
		_addr = src._addr;
		// to be completed if new attributes
	}
	return (*this);
}