#include "clientSocket.hpp"

/* SETTERS */ 
void            ClientSocket::setPort(unsigned short port)
{
    _port = port;
}

void            ClientSocket::setBuffer(std::string buffer)
{
    _buffer = buffer;
}

/* GETTERS */ 
unsigned short		&ClientSocket::getPort()
{
    return _port;
}

std::string  		&ClientSocket::getBuffer()
{
    return _buffer;
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
ClientSocket::ClientSocket() : 
					Socket(),
					_buffer("") 
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
	_buffer = src._buffer;
	return (*this);
}
