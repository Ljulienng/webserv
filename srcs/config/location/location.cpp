#include "location.hpp"

/* SETTERS */

void	Location::setLocationsDatas(std::map<std::string, std::string> mapLocation)
{
	std::map<std::string, std::string>::iterator it = mapLocation.begin();
	std::map<std::string, std::string>::iterator ite = mapLocation.end();
	
	while (it != ite)
	{
		if (it->first == "path") 
			setPath(it->second);
		else if (it->first == "root")
			setRoot(it->second);
		else if (it->first == "accepted_methods")
			setMethods(it->second);
		else if (it->first == "default_file")
			setDefaultFile(it->second);
		else if (it->first == "autoindex")
			setAutoindex(it->second);
		else if (it->first == "max_body_size")
			setMaxBodySize(it->second);
		else if (it->first == "redirection")
			setRedirections(it->second);
		// to be continued ...
		else
			throw (std::string("Error: unknown expression in configuration file"));
		it++;
	}
}

/* SETTERS (en cours) */
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

void	Location::setRedirections(std::string redirection)
{
	Str	tmp(redirection);

	if (tmp.getTokens().size() != 2)
		throw (std::string("Error: bad redirection format"));
	int statusCode = atoi(tmp.getTokens()[0].c_str());
	if (statusCode < 300 || statusCode > 399)
		throw (std::string("Error: bad redirection format"));
	_redirections.first = atoi(tmp.getTokens()[0].c_str());
	_redirections.second = tmp.getTokens()[1];
}

/* GETTERS */

std::vector<std::string>	&Location::getAcceptedMethod()
{ return _acceptedMethod; }

std::string		&Location::getPath()
{ return _path; }

std::string		&Location::getRoot()
{ return _root; }

bool	&Location::getAutoindex()
{ return _autoindex; }

size_t	&Location::getMaxBodySize()
{ return _maxBodySize; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Location::Location()  : _acceptedMethod(),
						_path(),
						_root(),
						_defaultFile(),
						_autoindex(),
						_maxBodySize(),
						_redirections()
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
		_redirections = src._redirections;
		// to be completed if new attributes
	}
	return (*this);
}