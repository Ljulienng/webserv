#include "socket.hpp"

/* GETTERS */

int						&Socket::getFd()
{ return _fd; }

std::string				&Socket::getServerName()
{ return _serverName; }

struct sockaddr_in 		&Socket::getSockaddr()
{ return _sockaddr; }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Socket::Socket() :
			_fd(),
			_sockaddr(),
			_serverName()
			// to be completed if new attributes
{}

Socket::Socket(int sockFd, struct sockaddr_in sockaddr, std::string serverName) :
			_fd(sockFd),
			_sockaddr(sockaddr),
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