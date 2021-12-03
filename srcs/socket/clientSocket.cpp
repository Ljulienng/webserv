#include "clientSocket.hpp"

/* SETTERS */ 
void            ClientSocket::setPort(unsigned short port)
{
    _port = port;
}

/* GETTERS */ 
unsigned short     &ClientSocket::getPort()
{
    return _port;
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
ClientSocket::ClientSocket() : Socket()
			// to be completed if new attributes
{}


ClientSocket::ClientSocket(const ClientSocket &src) : Socket()
{
	*this = src;
}

ClientSocket::~ClientSocket() {}

ClientSocket &ClientSocket::operator=(const ClientSocket &src)
{
	if (&src != this)
	{
        Socket::operator=(src);

		// to be completed if new attributes
	}
	return (*this);
}
