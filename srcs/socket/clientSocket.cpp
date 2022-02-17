#include "clientSocket.hpp"

/**/

void            ClientSocket::addRequest()
{
	Request newRequest(_buffer);
	
	_requests.push_back(newRequest);
	_buffer.clear();
}

void		ClientSocket::addResponse(Response* response)
{
	_responses.push_back(response);
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

void			ClientSocket::setRequests(std::list<Request> requests)
{
	_requests = requests;
}

void            ClientSocket::setServerName(std::string serverName)
{
	_serverName = serverName;
}

/* GETTERS */ 
unsigned short		&ClientSocket::getPort()
{ return _port; }

std::string  		&ClientSocket::getBuffer()
{ return _buffer; }

std::list<Request>	&ClientSocket::getRequests()
{ return _requests; }

std::list<Response*>    &ClientSocket::getResponses()
{ return		_responses; }

std::string			&ClientSocket::getServerName()
{ return _serverName; }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
ClientSocket::ClientSocket() :
			Socket(),
			_port(),
			_buffer(),
			_requests(),
			_responses(),
			_serverName()
			// to be completed if new attributes
{
	_type = client;
}

ClientSocket::ClientSocket(const ClientSocket &src) : Socket()
{
	*this = src;
}

ClientSocket::~ClientSocket()
{}

ClientSocket &ClientSocket::operator=(const ClientSocket &src)
{
	if (&src != this)
	{
        Socket::operator=(src);
		_port = src._port;
		_buffer = src._buffer;
		_requests = src._requests;
		_responses = src._responses;
		_serverName = src._serverName;
		// to be completed if new attributes
	}
	return (*this);
}
