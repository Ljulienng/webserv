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
							&Location::setIndex,
							&Location::setAutoindex ,
							&Location::setMaxBodySize,
							&Location::setRedirection };
	while (it != ite)
	{
		if ((ret = isValidExpression(it->first, locationExpression)) != -1)
			(this->*setData[ret])(it->second);
		else
			throw (std::string("Error [config file]: unknown expression in configuration file : " + it->first));
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
	Str		s(root);
	File	f(root);

	if (root == "" || s.getTokens().size() != 1)
		throw(std::string("Error [config file]: incorrect format on root directive"));
	if (!f.isDirectory())
		throw(std::string("Error [config file]: the root directive is not a directory"));
	_root = root;
}

void	Location::setMethods(std::string methods)
{
	Str	s(methods);

	if (s.getTokens().size() > 3)
		throw(std::string("Error: bad method"));
	for (size_t i = 0; i < s.getTokens().size(); i++)
		_acceptedMethod.push_back(s.getTokens()[i]);
}

void	Location::setIndex(std::string index)
{
	Str	s(index);
	
	if (index == "" || s.getTokens().size() != 1)
		throw(std::string("Error [config file]: incorrect format on index directive"));
	_index = index;
}

void	Location::setAutoindex(std::string autoIndex)
{
	if (autoIndex != "on" && autoIndex != "off")
		throw(std::string("Error [config file]: incorrect autoindex directive"));
	_autoindex = (autoIndex == "on");
}

void	Location::setMaxBodySize(std::string maxBodySize)
{
	Str val(maxBodySize);

	_maxBodySize = val.getNum();
}

void	Location::setRedirection(std::string redirection)
{
	Str	s(redirection);

	if (s.getTokens().size() != 2)
		throw (std::string("Error [config file]: incorrect redirection directive"));
	int statusCode = atoi(s.getTokens()[0].c_str());
	if (statusCode < 300 || statusCode > 399)
		throw (std::string("Error [config file]: incorrect redirection directive"));
	_redirection.first = atoi(s.getTokens()[0].c_str());
	_redirection.second = s.getTokens()[1];
}

/* GETTERS */
std::vector<std::string>	&Location::getAcceptedMethod()
{ return _acceptedMethod; }

std::string		&Location::getPath()
{ return _path; }

std::string		&Location::getRoot()
{ return _root; }

std::string					&Location::getIndex()
{ return _index; }

bool	&Location::getAutoindex()
{ return _autoindex; }

size_t	&Location::getMaxBodySize()
{ return _maxBodySize; }

std::pair<int, std::string>	&Location::getRedirection()
{ return _redirection; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Location::Location()  : _acceptedMethod(),
						_path(),
						_root(),
						_index(),
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
		_index = src._index;
		_autoindex = src._autoindex;
		_maxBodySize = src._maxBodySize;
		_redirection = src._redirection;
		// to be completed if new attributes
	}
	return (*this);
}

/* DEBUG */

void	Location::debug(size_t index)
{
	std::cout << "\t\tLOCATION[" << index << "] =>\n";
	std::cout << "\t\t - acceptedMethod = ";
	for (size_t i = 0; i < _acceptedMethod.size(); i++)
		std::cout << _acceptedMethod[i] << " ";
	std::cout << "\n\t\t - path = " << _path << "\n";
	std::cout << "\t\t - root = " << _root << "\n";
	std::cout << "\t\t - index = " << _index << "\n";
	std::cout << "\t\t - autoindex = " << _autoindex << "\n";
	std::cout << "\t\t - maxBodySize = " << _maxBodySize << "\n";
	std::cout << "\t\t - redirection = " << _redirection.first << " : " << _redirection.second  << "\n";
}