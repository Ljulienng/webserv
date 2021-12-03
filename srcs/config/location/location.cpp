#include "location.hpp"

/* SETTERS */
void	Location::setLocationsDatas(std::map<std::string, std::string> mapLocation)
{
	std::map<std::string, std::string>::iterator it = mapLocation.begin();
	std::map<std::string, std::string>::iterator ite = mapLocation.end();
	int ret;
	typedef void (Location::* funcPtr)(std::string);
	funcPtr setData[7] = {	&Location::setPath,
							&Location::setRoot,
							&Location::setMethods,
							&Location::setDefaultFile,
							&Location::setAutoindex ,
							&Location::setMaxBodySize,
							&Location::setRedirection };
	while (it != ite)
	{
		if ((ret = isValidExpression(it->first, locationExpression)) != -1)
			(this->*setData[ret])(it->second);
		else
			throw (std::string("Error: unknown expression in configuration file"));
		it++;
	}
}

/* SETTERS */
void	Location::setPath(std::string path)
{
	_path = path;
}

void	Location::setRoot(std::string root)
{
	Str	tmp(root);
	
	if (tmp.getTokens().size() != 1)
		throw(std::string("Error: bad root name"));
	_root = root;
}

void	Location::setMethods(std::string methods)
{
	Str	tmp(methods);

	if (tmp.getTokens().size() > 3)
		throw(std::string("Error: bad method"));
	for (size_t i = 0; i < tmp.getTokens().size(); i++)
		_acceptedMethod.push_back(tmp.getTokens()[i]);
}

void	Location::setDefaultFile(std::string default_file)
{
	Str	tmp(default_file);
	
	if (tmp.getTokens().size() != 1)
		throw(std::string("Error: bad default_file name"));
	_defaultFile = default_file;
}

void	Location::setAutoindex(std::string autoIndex)
{
	if (autoIndex != "on" && autoIndex != "off")
		throw(std::string("Error: bad autoindex"));
	_autoindex = (autoIndex == "on");
}

void	Location::setMaxBodySize(std::string maxBodySize)
{
	Str val(maxBodySize);

	_maxBodySize = val.getNum();
}

void	Location::setRedirection(std::string redirection)
{
	Str	tmp(redirection);

	if (tmp.getTokens().size() != 2)
		throw (std::string("Error: bad redirection format"));
	int statusCode = atoi(tmp.getTokens()[0].c_str());
	if (statusCode < 300 || statusCode > 399)
		throw (std::string("Error: bad redirection format"));
	_redirection.first = atoi(tmp.getTokens()[0].c_str());
	_redirection.second = tmp.getTokens()[1];
}

/* GETTERS */

std::vector<std::string>	&Location::getAcceptedMethod()
{ 
	return _acceptedMethod;
}

std::string		&Location::getPath()
{ 
	return _path;
}

std::string		&Location::getRoot()
{ 
	return _root;
}

bool	&Location::getAutoindex()
{
	return _autoindex;
}

size_t	&Location::getMaxBodySize()
{
	return _maxBodySize;
}

std::pair<int, std::string>	&Location::getRedirection()
{
	return _redirection;
}
/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Location::Location()  : _acceptedMethod(),
						_path(),
						_root(),
						_defaultFile(),
						_autoindex(),
						_maxBodySize(),
						_redirection()
						// to be completed if new attributes
{}

Location::Location(const Location &src)
{
    *this = src;
}

Location::~Location() {}

Location &Location::operator=(const Location &src)
{
	if (&src != this)
	{
		_acceptedMethod = src._acceptedMethod;
		_path = src._path;
		_root = src._root;
		_defaultFile = src._defaultFile;
		_autoindex = src._autoindex;
		_maxBodySize = src._maxBodySize;
		_redirection = src._redirection;
		// to be completed if new attributes
	}
	return (*this);
}