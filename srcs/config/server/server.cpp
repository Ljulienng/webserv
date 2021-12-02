#include "server.hpp"

// Basic socket initialization following the basic steps
int Server::start()
{
	createSocket();
	setNonBlocking();
	setSocketOptions();
	_socket.setAddr(AF_INET, _ip.c_str(), _port);
	bindSocket();
	
	// Listen
	if (listen(_sockfd, MAX_CONNECTIONS) < 0) // Maximum can be higher, to be tested
	{
		std::cerr << "Failed to listen on socket" << std::endl;
		return (EXIT_FAILURE);  
	}
	// Start (Not in this function)
	return (EXIT_SUCCESS);
}

void	Server::createSocket()
{
	_socket.setFd(socket(AF_INET, SOCK_STREAM, 0));
	if (_socket.getFd() == -1)
		throw(std::string("Failed to create socket"));
}

void	Server::setNonBlocking()
{
	if (fcntl(_socket.getFd(), F_SETFL, O_NONBLOCK) == -1)
		throw(std::string("Failed to set non blocking connection"));
}

/*
** set socket options
** set that the address is a reusable local address
** throw if the options can't be set to the socket.
*/
void Server::setSocketOptions()
{
	int option = 1;
	if (setsockopt(_socket.getFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) != 0)
		throw(std::string("Error: to set socket options"));
}

void	Server::bindSocket()
{
	if (bind(_socket.getFd(), (struct sockaddr *)&_socket.getSockaddr(), sizeof(_sockaddr)) < 0)
		{
			std::cerr << "Failed to bind " << std::endl;
			return (EXIT_FAILURE);
		}
}

void	Server::addLocation(Location location)
{
	_locations.push_back(location);
}

/* SETTERS */
void		Server::setServerDatas(std::map<std::string, std::string> mapServer)
{
	std::map<std::string, std::string>::iterator it = mapServer.begin();
	std::map<std::string, std::string>::iterator ite = mapServer.end();
	
	while (it != ite)
	{
		if (it->first == "server_name") 
			setName(it->second);
		else if (it->first == "ip")
			setIp(it->second);
		else if (it->first == "port")
			setPort(it->second);
		else if (it->first == "max_body_size")
			setMaxBodySize(it->second);
		// to be continued ...
		else
			throw (std::string("Error: unknown expression in configuration file"));
		it++;
	}
}

void	Server::setName(std::string name)
{
	Str	tmp(name);

	if (name == "" || tmp.getTokens().size() != 1)
		throw(std::string("Error: bad server name"));
	_name = name;
}

void	Server::setIp(std::string ip)
{
	if (ip.find_first_not_of("0123456789.") != std::string::npos)
		throw (std::string("Error: the ip is not well formatted"));
	if (std::count(ip.begin(), ip.end(), '.') != 3)
		throw (std::string("Error: the ip is not well formatted"));
	std::string::iterator it = ip.begin();
	for (; it != ip.end(); it++)
	{
		std::string::iterator start(it);
		while (*it != '.' && it != ip.end())
			it++;
		int nb = atoi(std::string(start, it).c_str());
		if (nb < 0 || nb > 254)
			throw (std::string("Error: the ip is not well formatted"));
		if (it == ip.end())
			break ;
	}
	_ip = ip;
}

void	Server::setPort(std::string port)
{
	if (port.find_first_not_of("0123456789") != std::string::npos)
		throw (std::string("Error: the port number has to be ... a number"));
	_port = static_cast<unsigned short>(atoi(port.c_str()));
}

void	Server::setMaxBodySize(std::string maxBodySize)
{
	Str val(maxBodySize);
	
	_maxBodySize = val.getNum();
}


/* GETTERS */

std::string		&Server::getName()
{ return _name; }

std::string		&Server::getIp()
{ return _ip; }

unsigned short		&Server::getPort()
{ return _port; }

std::vector<Location>	&Server::getLocations()
{ return _locations; }

size_t		&Server::getMaxBodySize()
{ return _maxBodySize; }

struct sockaddr_in 		&Server::getSockaddr()
{ return _sockaddr; }

int	&Server::getSockfd()
{ return _sockfd; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Server::Server() : 	_name(),
					_ip(),
					_port(),
					_maxBodySize(),
					_locations(),
					_sockaddr(),
					_sockfd()
					// to be completed if new attributes
{}

Server::Server(const Server &src)
{
	*this = src;
}

Server::~Server() {}

Server &Server::operator=(const Server &src)
{
	if (&src != this)
	{
		_name = src._name;
		_ip = src._ip;
		_port = src._port;
		_maxBodySize = src._maxBodySize;
		_locations = src._locations;
		_sockaddr = src._sockaddr;
		_sockfd = src._sockfd;
		// to be completed if new attributes
	}
	return (*this);
}