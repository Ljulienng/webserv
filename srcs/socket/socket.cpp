#include "socket.hpp"

/* SETTERS */
void        Socket::setFd(int fd)
{
	_pollFd.fd = fd;
}

void        Socket::setAddr(sockaddr_in addr)
{
	_addr = addr; 
}

void        Socket::setAddr(int family, const char *ip, unsigned short port)
{
	_addr.sin_family = family;
	_addr.sin_addr.s_addr = inet_addr(ip);
	_addr.sin_port = htons(port);
}

void        Socket::setPollFd(struct pollfd pollFd)
{ 
	_pollFd = pollFd;
}

void       Socket::setType(e_type type)
{
	_type = type;
}

void        Socket::setServerName(std::string serverName)
{
	_serverName = serverName;
}

/* GETTERS */
int						&Socket::getFd()
{ return _pollFd.fd; }

struct pollfd		    &Socket::getPollFd()
{ return _pollFd; }

struct sockaddr_in 		&Socket::getAddr()
{ return _addr; }

std::string 		    &Socket::getServerName()
{ return _serverName; }

Socket::e_type          &Socket::getType()
{ return _type; }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Socket::Socket() :
			_index(0),
			_fd(),
			_pollFd(),
			_addr(),
			_serverName(),
			_type(none)
			// to be completed if new attributes
{}

Socket::Socket(int sockFd, struct sockaddr_in sockaddr, e_type type) :	
			_index(0),
			_fd(sockFd),
			_pollFd(),
			_addr(sockaddr),
			_serverName(),
			_type(type)
			
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