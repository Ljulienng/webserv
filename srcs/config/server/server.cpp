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
	if (_socket.getFd() < 0)
		throw(std::string("Error: Failed to create socket"));
}

void	Server::setNonBlock()
{
	if (fcntl(_socket.getFd(), F_SETFL, O_NONBLOCK) < 0)
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

/*
** return a location bloc if : uri request = uri in location bloc
*/
Location	*Server::_findExactLocation(std::string uriRequest)
{
	for (size_t i = 0; i < _locations.size(); i++)
		if (_locations[i].getPath() == uriRequest)
			return &_locations[i];
	return NULL;
}

/*
** check if the location path match with the URI request
** for example "localhost:8080/images" matches with "location path = '/' "
** so we must have all the path in the request
*/
bool	_matchLocation(std::string locationPath, std::string uriRequest)
{
	if (uriRequest.size() < locationPath.size())
		return false;

	std::vector<std::string>	locList = tokenize(locationPath, "/");
	std::vector<std::string>	reqList = tokenize(uriRequest, "/");
	std::vector<std::string>::iterator loc = locList.begin();
	std::vector<std::string>::iterator req = reqList.begin();

	for ( ; req != reqList.end(); ++loc, ++req)
	{
		if (loc == locList.end())
			break ;
		if (loc != locList.end() && *loc != *req)
			return false;
	}
	return true;
}

/*
** it's possible that we have several location blocks which match the request
** so we have to determine which one is more precise
*/
bool	_isMorePreciseLocation(std::string locationPath, std::string prevLocationPath)
{
	//std::cout << "[_isMorePreciseLocation]   locpath = " << locationPath << "   prevLocationPath = " << prevLocationPath << "\n";
	return (locationPath.size() > prevLocationPath.size());
}

/*
** search a location block thanks to the uri (request)
*/
Location 	&Server::findLocation(std::string uriRequest)
{
	Location	*locationMatch = _findExactLocation(uriRequest);
	size_t 		indexMatch = 0;

	if (locationMatch != NULL)
		return *locationMatch;
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_matchLocation(_locations[i].getPath(), uriRequest))
		{
			if (_isMorePreciseLocation(_locations[i].getPath(), _locations[indexMatch].getPath()))
			{
				indexMatch = i;
				//std::cout << "Choose location = " << _locations[indexMatch].getPath() << "\n";
			}
		}
	}
	return _locations[indexMatch];
}

/* SETTERS */
void		Server::setServerDatas(std::map<std::string, std::string> mapServer)
{
	std::map<std::string, std::string>::iterator it = mapServer.begin();
	std::map<std::string, std::string>::iterator ite = mapServer.end();
	int ret;
	typedef void (Server::* funcPtr)(std::string);
	funcPtr setData[6] = {	&Server::setName,
							&Server::setIp,
							&Server::setPort,
							&Server::setRoot,
							&Server::setIndex,
							&Server::setMaxBodySize };
	while (it != ite)
	{
		if ((ret = isValidExpression(it->first, serverExpression)) != -1)
			(this->*setData[ret])(it->second);
		else
			throw (std::string("Error: unknown expression in configuration file : " + it->first));
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

// gerer cas d'erreurs
void	Server::setRoot(std::string root)
{
	_root = root;
}

// gerer cas d'erreurs
void	Server::setIndex(std::string index)
{
	_index = index;
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

std::string				&Server::getRoot()
{ return _root; }

std::string				&Server::getIndex()
{ return _index; }

std::vector<Location>	&Server::getLocations()
{ return _locations; }

size_t		&Server::getMaxBodySize()
{ return _maxBodySize; }

Socket 		&Server::getSocket()
{ return _socket; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Server::Server() : 	_name(),
					_ip(),
					_port(),
					_root(),
					_index(),
					_maxBodySize(1000000), // default nginx
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
		_root = src._root;
		_index = src._index;
		_maxBodySize = src._maxBodySize;
		_locations = src._locations;
		_socket = src._socket;
		// to be completed if new attributes
	}
	return (*this);
}

void	Server::debug(size_t index)
{
	std::cout << "\tSERVER[" << index << "] =>\n";
	std::cout << "\t - name = " << _name << "\n";
	std::cout << "\t - ip = " << _ip << "\n";
	std::cout << "\t - port = " << _port << "\n";
	std::cout << "\t - root = " << _root << "\n";
	std::cout << "\t - index = " << _index << "\n";
	std::cout << "\t - maxBodySize = " << _maxBodySize << "\n";
	std::cout << "\t - fd socket = " << _socket.getFd() << "\n";

	std::vector<Location>::iterator itLoc = _locations.begin();
	for (size_t i = 0; itLoc != _locations.end(); itLoc++, i++)
		itLoc->debug(i);
}