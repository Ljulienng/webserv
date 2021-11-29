#include "configuration.hpp"

void	Configuration::_parseConfigPath()
{
	File 	file(_configFile);

	if (_configFile.find(".conf") != _configFile.size() - 5 || _configFile.size() < 6)
		throw std::string("Error : the configuration file must be a <something.conf> file");
	if (file.isRegularFile() == false)
		throw std::string("Error : the configuration file is not a regular file");
	if (file.canReadFile() == false)
		throw std::string("Error : can't read configuration file");
}

/*
** remove spaces in the configuration file to make parsing easier
*/
void	Configuration::_cleanSpaces(std::string &buf)
{
	std::string::iterator it = buf.begin();
	std::string::iterator ite = buf.end();

	while (it != ite)
	{
		if (*it == '"')
		{
			it++;
			while (*it != '"' && it != buf.end())
				it++;
			it++;
		}
		else if (std::isspace(*it))
			it = buf.erase(it);
		else 
			it++;
	}
}

/*
** we find and insert a new pair,string,string> in the map m 
*/
size_t	Configuration::_parseNextPair(std::string::iterator it, std::string::iterator ite, std::map<std::string, std::string> &m)
{
	it++; // skip the first "
	std::string 						key;
	std::string 						value;
	std::string::iterator				start(it);

	while (*it != '"' && it != ite)
		it++;
	if (*it == '"' && *(it + 1) == ':' && *(it + 2) == '"')
	{
		key = std::string(start, it);
		it += 3;
	}
	else
		throw (std::string("Error: config file"));
	start = it;
	while (*it != '"' && it != ite)
		it++;
	if (*it == '"')
		value = std::string(start, it);
	m.insert(std::make_pair(key, value));
	// std::cout << "key = " << key << "  value = " << value << "\n";
	return (key.size() + value.size() + 5); // valeur cle + value + : + "" ""
}

bool	Configuration::_isBloc(std::string::iterator it, std::string::iterator ite, std::string blocName)	
{
	it++;
	std::string::iterator 	start(it);
	
	while (*it != '"' && it != ite)
		it++;
	if (std::string (start, it) == blocName && it != ite && *it == '"')
		return true;
	return false;
}

size_t	Configuration::_parseErrorPages(std::string::iterator it, std::string::iterator ite, std::string buf)
{
	std::map<std::string, std::string> 	tmp;
	std::string::iterator				start(it);
	
	it += 13; // skip "error_pages"
	while (*it != '{' && *(it + 1) != '"')
		it++;
	it++;
	while (*it != '}' && it != ite)
	{
		if (*it == '"')
			it += _parseNextPair(it, ite, tmp);
		if (*it == '}' || it == ite)
			break ;
		it++;
	}

	std::map<std::string, std::string>::iterator 	itmap = tmp.begin();
	for (; itmap != tmp.end(); itmap++)
	{
		int key = atoi(itmap->first.c_str());
		_errorPages.insert(std::pair<int, std::string>(key, itmap->second));
	}
	return (std::distance(start, it));
}

size_t	Configuration::_parseLocation(std::string::iterator it, std::string::iterator ite, std::string buf, Server &server)
{
	Location							location;
	std::map<std::string, std::string> 	mapLocation;
	std::string::iterator 				start(it);

	it += 10; // skip "location"
	while (*it != '}' && it != ite)
	{
		if (*it == '"')
		{
			it += _parseNextPair(it, ite, mapLocation);
			if (*it == '}' || it == ite)
				break ;
		}
		it++;
	}
	location.setLocationsDatas(mapLocation);
	server.addLocation(location);
	return (std::distance(start, it));
}

size_t	Configuration::_parseServer(std::string::iterator it, std::string::iterator ite, std::string buf)
{
	Server								server;
	std::map<std::string, std::string> 	mapServer;
	std::string::iterator 				start(it);

	it += 8; // skip "server"
	while (*it != '}' && it != ite)
	{
		if (*it == '"')
		{
			if (_isBloc(it, ite, "location"))
				it += _parseLocation(it, ite, buf, server);
			else
				it += _parseNextPair(it, ite, mapServer);
			if (*it == '}' || it == ite)
				break ;
		}
		it++;
	}
	
	server.setServerDatas(mapServer);
	_servers.insert(std::pair<std::string, Server>(server.getName(), server));
	
	// add locations to the new server
	std::vector<Location>::iterator itLoc = server.getLocations().begin();
	for (; itLoc != server.getLocations().end(); itLoc++)
		_servers.find(server.getName())->second.addLocation(*itLoc);

	return (std::distance(start, it));
}

void	Configuration::parse()
{
	std::ifstream 	fileStream(_configFile.c_str());
	std::string 	buf;

	std::getline(fileStream, buf, '\0');
	fileStream.close();
	_cleanSpaces(buf);
	// std::cout << "Buffer :\n" << buf << std::endl;

	std::map<std::string, std::string> 	mapConfig;
	std::string::iterator 				it = buf.begin();
	std::string::iterator 				ite = buf.end();

	// a faire : verifier la coherence syntaxique en 1er ({} [] "")
	if (*it++ != '{')
		throw std::string("Error: bad format : config scope");
	while (it != ite)
	{
		if (*it == '"')
		{
			if (_isBloc(it, ite, "server"))
				it += _parseServer(it, ite, buf);
			else if(_isBloc(it, ite, "error_pages"))
				it += _parseErrorPages(it, ite, buf);
			else
				it += _parseNextPair(it, ite, mapConfig);
			if (it == ite)
				break ;
		}
		it++;
	}
	setConfigDatas(mapConfig);
}

/*
** start socket for each server of the configuration file
*/
void		Configuration::startSockets()
{
	std::map<std::string, Server>::iterator	it = _servers.begin();

	for ( ; it != _servers.end(); it++)
		(*it).second.start();
}

/* SETTERS */

void	Configuration::setConfigDatas(std::map<std::string, std::string> mapConfig)
{
	std::map<std::string, std::string>::iterator it = mapConfig.begin();
	std::map<std::string, std::string>::iterator ite = mapConfig.end();

	while (it != ite)
	{
		if (it->first == "cgi")
			setCgi(it->second);
		else if (it->first == "max_body_size")
			setMaxBodySize(it->second);
		// to be continued ...
		else
			throw (std::string("Error: unknown expression in configuration file"));
		it++;
	}
}

void	Configuration::setCgi(std::string cgi)
{
	Str	cgiElements(cgi);

	if (cgiElements.getTokens().size() != 2 || cgiElements.getTokens()[0][0] != '.')
		throw (std::string("Error: bad cgi format in configuration file"));
	_cgi.first = cgiElements.getTokens()[0];
	_cgi.second = cgiElements.getTokens()[1];
}

void	Configuration::setMaxBodySize(std::string maxBodySize)
{
	Str val(maxBodySize);

	_maxBodySize = val.getNum();
}



/* GETTERS */

std::pair<std::string, std::string>		&Configuration::getCgi()
{ return _cgi; }

size_t		&Configuration::getMaxBodySize()
{ return _maxBodySize; }

std::map<int, std::string>		&Configuration::getErrorPages()
{ return _errorPages; }

std::map<std::string, Server>		&Configuration::getServers()
{ return _servers; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Configuration::Configuration() : 	_configFile(),
									_cgi(),
									_maxBodySize(),
									_errorPages(),
									_servers()
									// to be completed if new attributes
{}

Configuration::Configuration(std::string configFile) : _configFile(configFile)
{
	_parseConfigPath();
}

Configuration::Configuration(const Configuration &src)
{
	*this = src;
}

Configuration::~Configuration() {}

Configuration &Configuration::operator=(const Configuration &src)
{
	if (&src != this)
	{
		_configFile = src._configFile;
		_cgi = src._cgi;
		_maxBodySize = src._maxBodySize;
		_errorPages = src._errorPages;
		_servers = src._servers;
		// to be completed if new attributes
	}
	return (*this);
}


/* DEBUG */

void	Configuration::debug()
{
	std::cout << "\n***** DEBUG *****\n";
	std::cout << "CONFIG (general scope) =>\n";
	std::cout << " - cgi =  1->" << _cgi.first << "  2->" << _cgi.second << "\n";
	std::cout << " - maxBodySize = " << _maxBodySize << "\n";
	std::map<int, std::string>::iterator itErr = _errorPages.begin();
	for (; itErr != _errorPages.end(); itErr++)
		std::cout << " - errorPages = " << itErr->first << ":" << itErr->second << "\n";
	
	std::map<std::string, Server>::iterator itServ = _servers.begin();
	int i = 0;
	for (; itServ != _servers.end(); itServ++)
	{
		std::cout << "\tSERVER[" << i << "] =>\n";
		std::cout << "\t - name = " << itServ->second.getName() << "\n";
		std::cout << "\t - ip = " << itServ->second.getIp() << "\n";
		std::cout << "\t - port = " << itServ->second.getPort() << "\n";
		std::cout << "\t - maxBodySize = " << itServ->second.getMaxBodySize() << "\n";

		std::vector<Location>::iterator itLoc = itServ->second.getLocations().begin();
		int j = 0;
		for (; itLoc != itServ->second.getLocations().end(); itLoc++)
		{
			std::cout << "\t\tLOCATION[" << j << "] =>\n";
			std::cout << "\t\t - acceptedMethod = ";
			for (int k = 0; k < itLoc->getAcceptedMethod().size(); k++)
				std::cout << itLoc->getAcceptedMethod()[k] << " ";
			std::cout << "\n\t\t - path = " << itLoc->getPath() << "\n";
			std::cout << "\t\t - root = " << itLoc->getRoot() << "\n";
			std::cout << "\t\t - autoindex = " << itLoc->getAutoindex() << "\n";
			std::cout << "\t\t - maxBodySize = " << itLoc->getMaxBodySize() << "\n";
			j++;
		}

		i++;
	}
}