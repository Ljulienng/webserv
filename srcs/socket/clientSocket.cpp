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

void	ClientSocket::setTimeout()
{
	clock_gettime(CLOCK_MONOTONIC, &_lastEvent);
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

bool	ClientSocket::getTimeout()
{
	struct timespec t;
	double			elapsedTime;
	
	clock_gettime(CLOCK_MONOTONIC, &t);
	elapsedTime = (t.tv_sec - _lastEvent.tv_sec) * 1e9;
	elapsedTime = (elapsedTime + (t.tv_nsec - _lastEvent.tv_nsec)) * 1e-9;

	return (elapsedTime > 120);
}	

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
	setTimeout();
}

ClientSocket::ClientSocket(const ClientSocket &src) : Socket()
{
	*this = src;
}

ClientSocket::~ClientSocket()
{
	std::list<Response*>::iterator it = _responses.begin();
    for ( ; it != _responses.end(); it++)
    {
		delete *it;
		*it = NULL;
	}
}

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