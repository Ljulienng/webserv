#include "clientSocket.hpp"


/**/

void            ClientSocket::addRequest()
{
	Request newRequest(_buffer);
	
	if (newRequest.getRet() == 200)
		_requests.push(newRequest);
	else
		std::cerr << "Incorrect Request, error: " << newRequest.getRet() << std::endl;
	// else
	// {
	// 	std::cerr << "buffer = \n" << _buffer;
	// 	std::cerr << "Incorrect Request: " << newRequest.getRet() << std::endl;
	// }
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

void            ClientSocket::setServerName(std::string serverName)
{
	_serverName = serverName;
}

/* GETTERS */ 
unsigned short		&ClientSocket::getPort()
{ return _port; }

std::string  		&ClientSocket::getBuffer()
{ return _buffer; }

std::queue<Request>	&ClientSocket::getRequests()
{ return _requests; }

std::queue<Response>    &ClientSocket::getResponses()
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
		_port = src._port;
		_buffer = src._buffer;
		_requests = src._requests;
		_responses = src._responses;
		_serverName = src._serverName;
		// to be completed if new attributes
	}
	return (*this);
}
