#include "clientSocket.hpp"


void            ClientSocket::addRequest()
{
	Request newRequest(_buffer);

	_requests.push(newRequest);
	_buffer.clear();
}

void		ClientSocket::addResponse(Response response)
{
	_responses.push(response);
}

/* SETTERS */ 
void 		ClientSocket::setPort(unsigned short port)
{
    _port = port;
}

void            ClientSocket::setBuffer(std::string buffer)
{
    _buffer = buffer;
}

void			ClientSocket::setRequests(std::queue<Request> requests)
{
	_requests = requests;
}

/* GETTERS */ 
unsigned short     &ClientSocket::getPort()
{
    return _port;
}

std::string  		&ClientSocket::getBuffer()
{
    return _buffer;
}

std::queue<Request>	&ClientSocket::getRequests()
{
	return _requests;
}

std::queue<Response>    &ClientSocket::getResponses()
{
	return		_responses;
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
ClientSocket::ClientSocket() : Socket(), _port(), _responses()
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
