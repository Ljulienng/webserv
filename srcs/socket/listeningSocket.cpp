#include "listeningSocket.hpp"

std::string	ListeningSocket::_printError()
{
	switch (_state)
	{
		case NONBLOCK : return "Error: failed to set non blocking connection";
		case SETSOCKOPT : return "Error: failed to set socket options";
		case BIND : return "Error: failed to bind";
		case LISTEN : return "Error: failed to listen on socket";
		default: return "no error";
	}
}

int 	ListeningSocket::start(std::string ip, unsigned short port)
{
	setNonBlock();
	setSocketOptions();
	setAddr(AF_INET, ip.c_str(), port);
	bindSocket();
	listenSocket();
	_pollFd.events = POLLIN;
	if (_state != NONE)
		std::cerr << _printError() << std::endl;
	return _state;
}

/*
** F_SETFL set the file status flags to the value specified by arg [O_NONBLOCK]
** with O_NONBLOCK, system call fails (and without it, system call is blocked until the lock is removed)
*/
int	ListeningSocket::setNonBlock()
{
	if (fcntl(_pollFd.fd, F_SETFL, O_NONBLOCK) < 0)
		return (_state = NONBLOCK);
	return (EXIT_SUCCESS);
}

/*
** SO_REUSEADDR is to allow to reuse the address even if the process crash or been killed
*/
int 	ListeningSocket::setSocketOptions()
{
	int option = 1;
	if (setsockopt(_pollFd.fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) != 0)
		return (_state = SETSOCKOPT);
	return (EXIT_SUCCESS);
}

/*
** assigns a name to the socket
** must be a unique ip/port pair otherwise error (address already in use.)
*/
int	ListeningSocket::bindSocket()
{
	if (bind(_pollFd.fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
		return (_state = BIND);
	return (EXIT_SUCCESS);
}

/*
** listen for socket connections
*/
int	ListeningSocket::listenSocket()
{
	if (listen(_pollFd.fd, MAX_CONNECTIONS) < 0)
		return (_state = LISTEN);
	return (EXIT_SUCCESS);
}


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
ListeningSocket::ListeningSocket(std::string serverName) : Socket(), _state(NONE)
			// to be completed if new attributes
{
	_type = server;
	_serverName = serverName;
}

ListeningSocket::ListeningSocket(const ListeningSocket &src) : Socket()
{
	*this = src;
}

ListeningSocket::~ListeningSocket()
{
	if (_pollFd.fd != 0)
	{
		close(_pollFd.fd);
		_pollFd.fd = 0;
	}
}

ListeningSocket &ListeningSocket::operator=(const ListeningSocket &src)
{
	if (&src != this)
	{
        Socket::operator=(src);
		_state = src._state;
		// to be completed if new attributes
	}
	return (*this);
}
