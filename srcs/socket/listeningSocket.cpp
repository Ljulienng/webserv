#include "listeningSocket.hpp"

void 	ListeningSocket::start(std::string ip, unsigned short port)
{
	createSocket();
	setNonBlock();
	setSocketOptions();
	std::cout << "ip = " << ip << "   port = " << port << "\n";
	setAddr(AF_INET, ip.c_str(), port);
	bindSocket();
	listenSocket();
	_pollFd.events = POLLIN;
}

void	ListeningSocket::createSocket()
{
	// _pollFd.fd = socket(AF_INET, SOCK_STREAM, 0);
	// std::cout << "fd = " << _pollFd.fd << "\n";
	// if (_pollFd.fd < 0)
	// 	throw(std::string("Error: Failed to create socket"));
}

void	ListeningSocket::setNonBlock()
{
	if (fcntl(_pollFd.fd, F_SETFL, O_NONBLOCK) < 0)
		throw(std::string("Error: Failed to set non blocking connection"));
}

/*
** set that the address is a reusable local address
** throw if the options can't be set to the socket.
*/
void 	ListeningSocket::setSocketOptions()
{
	int option = 1;
	if (setsockopt(_pollFd.fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) != 0)
		throw(std::string("Error: to set socket options"));
}

void	ListeningSocket::bindSocket()
{
	if (bind(_pollFd.fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
		throw(std::string("Error: Failed to bind"));
}

void	ListeningSocket::listenSocket()
{
	if (listen(_pollFd.fd, MAX_CONNECTIONS) < 0) // Maximum can be higher, to be tested
		throw(std::string("Error: Failed to listen on socket"));
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
ListeningSocket::ListeningSocket() : Socket()
			// to be completed if new attributes
{
	_type = server;
}

ListeningSocket::ListeningSocket(std::string serverName) : Socket()
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
		// to be completed if new attributes
	}
	return (*this);
}
