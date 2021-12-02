#include "server.hpp"

// Basic socket initialization following the basic steps
void 	Server::start()
{
	createSocket();
	setNonBlock();
	setSocketOptions();
	_socket.setAddr(AF_INET, _ip.c_str(), _port);
	bindSocket();
	listenSocket();
}

void	Server::createSocket()
{
	_socket.setFd(socket(AF_INET, SOCK_STREAM, 0));
	if (_socket.getFd() == -1)
		throw(std::string("Error: Failed to create socket"));
}

void	Server::setNonBlock()
{
	if (fcntl(_socket.getFd(), F_SETFL, O_NONBLOCK) == -1)
		throw(std::string("Error: Failed to set non blocking connection"));
}

/*
** set that the address is a reusable local address
** throw if the options can't be set to the socket.
*/
void 	Server::setSocketOptions()
{
	int option = 1;
	if (setsockopt(_socket.getFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) != 0)
		throw(std::string("Error: to set socket options"));
}

void	Server::bindSocket()
{
	if (bind(_socket.getFd(), (struct sockaddr *)&_socket.getAddr(), sizeof(_socket.getAddr())) < 0)
			throw(std::string("Error: Failed to bind"));
}

void	Server::listenSocket()
{
	if (listen(_socket.getFd(), MAX_CONNECTIONS) < 0) // Maximum can be higher, to be tested
		throw(std::string("Error: Failed to listen on socket"));
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
	int ret;
	typedef void (Server::* funcPtr)(std::string);
	funcPtr setData[4] = {	&Server::setName,
							&Server::setIp,
							&Server::setPort,
							&Server::setMaxBodySize };
	while (it != ite)
	{
		if ((ret = isValidExpression(it->first, serverExpression)) != -1)
			(this->*setData[ret])(it->second);
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
{
	return _name;
}

std::string		&Server::getIp()
{
	return _ip;
}

unsigned short		&Server::getPort()
{
	return _port;
}

std::vector<Location>	&Server::getLocations()
{
	return _locations;
}

size_t		&Server::getMaxBodySize()
{
	return _maxBodySize;
}

Socket 		&Server::getSocket()
{
	return _socket;
}


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Server::Server() : 	_name(),
					_ip(),
					_port(),
					_maxBodySize(),
					_locations(),
					_socket()
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
		_socket = src._socket;
		// to be completed if new attributes
	}
	return (*this);
}