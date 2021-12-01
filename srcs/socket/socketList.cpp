#include "socketList.hpp"


void	SocketList::addSocket(int sockFd, struct sockaddr_in sockaddr, std::string serverName)
{
    (void)sockFd;
    (void)sockaddr;
    (void)serverName;
}

/* GETTERS */



/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

SocketList::SocketList() 
			// to be completed if new attributes
{}

SocketList::SocketList(const SocketList &src)
{
	*this = src;
}

SocketList::~SocketList()
{
	
}

SocketList &SocketList::operator=(const SocketList &src)
{
	if (&src != this)
	{
	
		// to be completed if new attributes
	}
	return (*this);
}