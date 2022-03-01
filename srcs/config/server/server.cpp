#include "server.hpp"

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
				indexMatch = i;
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
	funcPtr setData[8] = {	&Server::setName,
							&Server::setIp,
							&Server::setPort,
							&Server::setRoot,
							&Server::setIndex,
							&Server::setMaxBodySize,
							&Server::setUploadPath,
							&Server::setCgi };
	while (it != ite)
	{
		if ((ret = isValidExpression(it->first, serverExpression)) != -1)
			(this->*setData[ret])(it->second);
		else
			throw (std::string("Error [config file]: unknown expression in configuration file : " + it->first));
		it++;
	}
}

void	Server::setName(std::string name)
{
	Str	s(name);

	if (name == "" || s.getTokens().size() != 1)
		throw(std::string("Error [config file]: incorrect format on server name"));
	_name = name;
}

void	Server::setIp(std::string ip)
{
	if (ip.find_first_not_of("0123456789.") != std::string::npos)
		throw (std::string("Error [config file]: incorrect format on IP"));
	if (std::count(ip.begin(), ip.end(), '.') != 3)
		throw (std::string("Error [config file]: incorrect format on IP"));
	std::string::iterator it = ip.begin();
	for (; it != ip.end(); it++)
	{
		std::string::iterator start(it);
		while (*it != '.' && it != ip.end()) 
			it++;
		int nb = atoi(std::string(start, it).c_str());
		if (nb < 0 || nb > 254)
			throw (std::string("Error: incorrect format on IP"));
		if (it == ip.end())
			break ;
	}
	_ip = ip;
}

void	Server::setPort(std::string port)
{
	if (port.find_first_not_of("0123456789") != std::string::npos)
		throw (std::string("Error [config file]: incorrect format on port number"));
	if (port.length() > 5)
		throw (std::string("Error [config file]: incorrect format on port number"));
	_port = static_cast<unsigned short>(atoi(port.c_str()));
}

void	Server::setRoot(std::string root)
{	
	Str		s(root);
	File	f(root);

	if (root == "" || s.getTokens().size() != 1)
		throw(std::string("Error [config file]: incorrect format on root directive"));
	if (!f.isDirectory())
		throw(std::string("Error [config file]: the root directive is not a directory"));
	_root = root;
}

void	Server::setIndex(std::string index)
{
	Str		s(index);

	if (index == "" || s.getTokens().size() != 1)
		throw(std::string("Error [config file]: incorrect format on index directive"));
	_index = index;
}

void	Server::setMaxBodySize(std::string maxBodySize)
{
	Str val(maxBodySize);

	_maxBodySize = val.getNum();
}

void		Server::setUploadPath(std::string uploadPath)
{
	File	f(_root + uploadPath);

	if (!f.isDirectory())
		throw(std::string("Error [config file]: the upload_path directive is not a directory"));
	_uploadPath = uploadPath;
}

void	Server::setCgi(std::string cgi)
{
	Str	cgiElements(cgi);
	
	if (cgiElements.getTokens().size() != 2 || cgiElements.getTokens()[0][0] != '.')
		throw (std::string("Error [config file]: incorrect format on CGI"));
	_cgi.first = cgiElements.getTokens()[0];
	_cgi.second = cgiElements.getTokens()[1];

	File cgiExec(_cgi.second);
	
	if (!cgiExec.isRegularFile())
		throw (std::string("Error [config file]: incorrect format on CGI"));
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

std::string		&Server::getUploadPath()
{ return _uploadPath; }

std::pair<std::string, std::string>		&Server::getCgi()
{ return _cgi; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Server::Server() : 	_name(),
					_ip(),
					_port(),
					_root(),
					_index(),
					_maxBodySize(100000),
					_uploadPath(),
					_locations(),
					_cgi()
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
		_uploadPath = src._uploadPath;
		_locations = src._locations;
		_cgi = src._cgi;
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
	std::cout << "\t - uploadPath = " << _uploadPath << "\n";
	std::cout << "\t - cgi =  1->" << _cgi.first << "  2->" << _cgi.second << "\n";

	std::vector<Location>::iterator itLoc = _locations.begin();
	for (size_t i = 0; itLoc != _locations.end(); itLoc++, i++)
		itLoc->debug(i);
}