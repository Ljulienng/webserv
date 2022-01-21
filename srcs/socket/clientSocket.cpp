#include "clientSocket.hpp"


/**/
int				ClientSocket::verifBuffer()
{
	std::cout << _buffer;
	if (_buffer.find("\r\n\r\n") == std::string::npos)
		return (1);
	if (_buffer.find("Content-Length: ") == std::string::npos)
	{
		if (_buffer.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			size_t i = _buffer.find("0\r\n\r\n");
			if (i != std::string::npos && i == _buffer.size() - 5)
				return (0);
			else
				return (1);
		}
		else
			return (1);
	}
	else
	{
		size_t contentLength = atoi(_buffer.substr(_buffer.find("Content-Length: "), 26).c_str());
		std::string	body = _buffer.substr(_buffer.find("\r\n\r\n") + 4, std::string::npos);

		if (body.size() > contentLength)
		{
			// std::cerr << "Body size exceeds the content-length specified\n";
			return (1);
		}
		return (0);
	}
}

void            ClientSocket::addRequest()
{
	Request newRequest(_buffer);

	if (newRequest.getRet() == 200)
		_requests.push(newRequest);
	else
		std::cerr << "Incorrect Request\n";
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
