#include "socketList.hpp"

void	SocketList::addSocket(Socket* newSocket)
{
    _sockets.push_back(newSocket);
}

/* GETTERS */

std::list<Socket*>		SocketList::getSockets()
{ return _sockets; }


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