#include "listeningSocket.hpp"

int 	ListeningSocket::start(std::string ip, unsigned short port)
{
	setNonBlock();
	setSocketOptions();
	setAddr(AF_INET, ip.c_str(), port);
	bindSocket();
	listenSocket();
	_pollFd.events = POLLIN;
	return _state;
}

int	ListeningSocket::setNonBlock()
{
	if (fcntl(_pollFd.fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << RED << "Error: failed to set non blocking connection" << std::endl;
		return (_state = NONBLOCK);
	}
	return (EXIT_SUCCESS);
}

/*
** set that the address is a reusable local address
** throw if the options can't be set to the socket.
*/
int 	ListeningSocket::setSocketOptions()
{
	int option = 1;
	if (setsockopt(_pollFd.fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) != 0)
	{
		std::cerr << RED << "Error: to set socket options" << std::endl;
		return (_state = SETSOCKOPT);
	}
	return (EXIT_SUCCESS);
}

int	ListeningSocket::bindSocket()
{
	if (bind(_pollFd.fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
	{
		std::cerr << RED << "Error: failed to bind" << std::endl;
		return (_state = BIND);
	}
	return (EXIT_SUCCESS);
}

int	ListeningSocket::listenSocket()
{
	if (listen(_pollFd.fd, MAX_CONNECTIONS) < 0) // Maximum can be higher, to be tested
	{
		std::cerr << RED << "Error: failed to listen on socket" << std::endl;
		return (_state = LISTEN);
	}
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

ListeningSocket::~ListeningSocket() {}

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
