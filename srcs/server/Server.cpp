#include "Server.hpp"

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
	_sockaddr = src._sockaddr;
	_sockfd = src._sockfd;

	return (*this);
}

int Server::start() //Initialisation du socket
{
	// Socket
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1)
	{
		std::cerr << "Failed to create socket." << std::endl; // Message temporaire
		return (EXIT_FAILURE);
	}
	// Bind
	fillAddr();
	if (bind(_sockfd, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr) < 0))
	{
		std::cerr << "Failed to bind" << std::endl;
		return (EXIT_FAILURE);
	}
	// Listen
	if (listen(_sockfd, 100) < 0) // Maximum modifiable à voir
	{
		std::cerr << "Failed to listen on socket" << std::endl;
		return (EXIT_FAILURE);
	}
	// Start à faire dans une autre fonction
	return (EXIT_SUCCESS);
}

void Server::fillAddr()
{
	memset((char *)&_sockaddr, 0, sizeof(_sockaddr));
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_addr.s_addr = htonl(HOST);
	_sockaddr.sin_family = htons(PORT);
	// PORT ET HOST à définir
}