#include "configuration.hpp"

void 		Configuration::parseConfigPath(std::string configFile)
{
	_configFile = configFile;

	File 	file(_configFile);

	if (_configFile.find(".conf") != _configFile.size() - 5 || _configFile.size() < 6)
		throw std::string("Error : the configuration file must be a <something.conf> file");
	if (file.isRegularFile() == false)
		throw std::string("Error : the configuration file is not a regular file");
	if (file.canReadFile() == false)
		throw std::string("Error : can't read configuration file");
}

// void	Configuration::_parseConfigPath()
// {
// 	File 	file(_configFile);

// 	if (_configFile.find(".conf") != _configFile.size() - 5 || _configFile.size() < 6)
// 		throw std::string("Error : the configuration file must be a <something.conf> file");
// 	if (file.isRegularFile() == false)
// 		throw std::string("Error : the configuration file is not a regular file");
// 	if (file.canReadFile() == false)
// 		throw std::string("Error : can't read configuration file");
// }

/*
** remove spaces in the configuration file except beetween 2 quotes to make parsing easier
*/
void	Configuration::_cleanSpaces(std::string &buf)
{
	str_ite it = buf.begin();
	str_ite ite = buf.end();

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
** we find and insert a new pair<string,string> into the map m 
*/
size_t	Configuration::_parseNextPair(str_ite it, str_ite ite, std::map<std::string, std::string> &m)
{
	it++; // skip the first "
	std::string 		key;
	std::string 		value;
	str_ite				start(it);

	while (*it != '"' && it != ite)
		it++;
	if (*it == '"' && *(it + 1) == ':' && *(it + 2) == '"')
	{
		key = std::string(start, it);
		it += 3;
	}
	else
		throw (std::string("Error: config file : " + std::string(it, it + 5)));
	start = it;
	while (*it != '"' && it != ite)
		it++;
	if (*it == '"')
		value = std::string(start, it);
	m.insert(std::make_pair(key, value));
	// std::cout << "key = " << key << "  value = " << value << "\n";
	return (key.size() + value.size() + 5); // key + value + : + "" ""
}

bool	Configuration::_isBloc(str_ite it, str_ite ite, std::string blocName)	
{
	it++;
	str_ite 	start(it);
	
	while (*it != '"' && it != ite)
		it++;
	if (std::string (start, it) == blocName && it != ite && *it == '"')
		return true;
	return false;
}

size_t	Configuration::_parseErrorPages(str_ite it, str_ite ite)
{
	std::map<std::string, std::string> 	tmp;
	str_ite				start(it);
	
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

size_t	Configuration::_parseLocation(str_ite it, str_ite ite, Server &server)
{
	Location							location;
	std::map<std::string, std::string> 	mapLocation;
	str_ite 							start(it);

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
	server.addLocation(location); // add location to the new server
	return (std::distance(start, it));
}

size_t	Configuration::_parseServer(str_ite it, str_ite ite)
{
	Server								server;
	std::map<std::string, std::string> 	mapServer;
	str_ite 							start(it);

	while (*it != '}' && it != ite)
	{
		if (*it == '"')
		{
			if (_isBloc(it, ite, "location"))
			{
				it += 10; // skip location"
				it += _parseLocation(it, ite, server);
				while (*it != ']' && it != ite)
				{
					while (*it != '"' && *it != ']' && it != ite)
						it++;
					if (*it == '"')
					{
						str_ite	end(++it);
						for (; *end != '"'; end++);
						if (isValidExpression(std::string(it--, end), locationExpression) != -1)
							it += _parseLocation(it, ite, server);
						it++;
					}
				}
			}
			else
				it += _parseNextPair(it, ite, mapServer);
			if (*it == '}' || it == ite)
				break ;
		}
		it++;
	}
	std::cout << "map size = " << mapServer.size() << "\n";
	server.setServerDatas(mapServer);

	// add the new server to the array of servers
	_servers.push_back(server);

	return (std::distance(start, it));
}

bool	Configuration::_checkConfigScope(str_ite it, str_ite ite)
{
	size_t	openCurlyBracket = std::count(it, ite, '{');
	size_t	closeCurlyBracket = std::count(it, ite, '}');
	size_t	openSquareBracket = std::count(it, ite, '[');
	size_t	closeSquareBracket = std::count(it, ite, ']');
	size_t	dbQuote = std::count(it, ite, '"');

	if (openCurlyBracket != closeCurlyBracket || openSquareBracket != closeSquareBracket || dbQuote % 2 != 0)
		return false;
	return true;
}

void	Configuration::parse()
{
	std::ifstream 	fileStream(_configFile.c_str());
	std::string 	buf;

	std::getline(fileStream, buf, '\0');
	fileStream.close();
	_cleanSpaces(buf);
	//std::cout << "Buffer :\n" << buf << std::endl;

	std::map<std::string, std::string> 	mapConfig;
	str_ite 							it = buf.begin();
	str_ite 							ite = buf.end();

	if (!_checkConfigScope(it, ite))
		throw std::string("Error: bad format : config scope");
	//std::cout << "it = " << *(it + 2) << "\n";
	if (*it++ != '{')
		throw std::string("Error: bad format : config scope");
	while (it != ite)
	{
		if (*it == '"')
		{
			if (_isBloc(it, ite, "server"))
			{
				it += 8; // skip server"
				it += _parseServer(it, ite);
				while (*it != ']')
				{
					while (*it != '"' && *it != ']' && it != ite)
						it++;
					if (*it == '"')
					{
						str_ite		end(++it);
						for (; *end != '"'; end++);
						if (isValidExpression(std::string(it--, end), serverExpression) != -1) // si prochain mot fait partie des mots cles (definir enum)
							it += _parseServer(it, ite);
						it++;
					}
				}
			}
			else if(_isBloc(it, ite, "error_pages"))
			{
				it += 13; // skip "error_pages"
				it += _parseErrorPages(it, ite);
			}
			else
				it += _parseNextPair(it, ite, mapConfig);
			if (it == ite)
				break ;
		}
		it++;
	}
	// setConfigDatas(mapConfig);
}

/*
** search a server thanks to its name
*/
Server 		&Configuration::findServer(std::string serverName)
{
	Server *serverMatch = &_servers[0];

	for (size_t i = 0; i < _servers.size(); i++)
		if (_servers[i].getName() == serverName)
			serverMatch = &_servers[i];
	return *serverMatch;
}

/* GETTERS */
std::map<int, std::string>		&Configuration::getErrorPages()
{ return _errorPages; }

std::vector<Server>		&Configuration::getServers()
{ return _servers; }

std::vector<ClientSocket>	&Configuration::getClients()
{ return _clients; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Configuration::Configuration() : Singleton(),
									_configFile(),
									_errorPages(),
									_servers(),
									_clients()
									// to be completed if new attributes
{}

Configuration::Configuration(std::string configFile) :
									Singleton(),
									_configFile(configFile),
									_errorPages(),
									_servers(),
									_clients()					
									// to be completed if new attributes
{
	// _parseConfigPath();
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
		_errorPages = src._errorPages;
		_servers = src._servers;
		_clients = src._clients;
		// to be completed if new attributes
	}
	return (*this);
}

/* NON MEMBER FUNCTIONS */

int isValidExpression(std::string expression, const char **validExpressions)
{
	size_t i = 0;

	while (validExpressions[i])
	{
		if (expression == validExpressions[i])
			return (i);
		++i;
	}
	return (-1);
}

/* DEBUG */

void	Configuration::debug()
{
	std::cout << "\n***** DEBUG *****\n";
	std::cout << "CONFIG (general scope) =>\n";
	std::map<int, std::string>::iterator itErr = _errorPages.begin();
	for (; itErr != _errorPages.end(); itErr++)
		std::cout << " - errorPages = " << itErr->first << ":" << itErr->second << "\n";
	for (size_t i = 0; i < _servers.size(); i++)
		_servers[i].debug(i);
}