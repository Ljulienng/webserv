#include "Server.hpp"

// Basic socket initialization following the basic steps
int Server::start()
{
	// Socket
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1)
	{
		std::cerr << "Failed to create socket." << std::endl; // Temporary messages
		return (EXIT_FAILURE);
	}
	// Bind
	initAddr();
	if (bind(_sockfd, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr) < 0))
	{
		std::cerr << "Failed to bind" << std::endl;
		return (EXIT_FAILURE);
	}
	// Listen
	if (listen(_sockfd, 100) < 0) // Maximum can be higher, to be tested
	{
		std::cerr << "Failed to listen on socket" << std::endl;
		return (EXIT_FAILURE);
	}
	// Start (Not in this function)
	return (EXIT_SUCCESS);
}

// Filling the sockaddr_in variable
void Server::initAddr()
{
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_addr.s_addr = htonl(HOST);
	_sockaddr.sin_family = htons(PORT);
	// PORT & HOST from the config file
}

int	Server::getSockfd()
{
	return (_sockfd);
}

Server::Server()
{
}

Server::Server(const Server &src)
{
	*this = src;
}

Server::~Server()
{
}

Server &Server::operator=(const Server &src)
{
	if (&src == this)
		return (*this);
	_sockaddr = src._sockaddr;
	_sockfd = src._sockfd;

	return (*this);
}