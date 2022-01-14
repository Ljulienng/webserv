#include "uri.hpp"

/* PARSER */

void 	Uri::createUrl(std::map<std::string, std::string> headers, std::string path)
{
	std::string url;
	std::string scheme = "http";
	std::string host = headers["Host"];

	if (_authority._host == "" && host != "")
		_authority._host = host;
	if (_path == "" && path != "")
		_path = path;
	url = scheme + "://" + _authority._host + path;
	// std::cout << "url = " << url << "\n";
	_url = url;
}

void	Uri::urlParser(const std::string &url)
{
	std::string		newUrl;
	size_t 			index = url.find_first_of(":");
	bool			has_autho;

	_scheme = url.substr(0, index);
			std::cout << "URL = " << url << std::endl;
	url.find("://") != std::string::npos ? has_autho = true : has_autho = false;
	if (has_autho == true)
	{
		newUrl = url.substr(index + 3);
		if (newUrl.find_first_of("@") != std::string::npos)
		{
			index = newUrl.find_first_of("@");
			_authority._userInfo = newUrl.substr(0, index);
			newUrl.assign(newUrl, index, std::string::npos);
		}
		if (newUrl.find_first_of(":") != std::string::npos)
		{
			index = newUrl.find_first_of(":");
			_authority._host = newUrl.substr(0, index);
			newUrl.assign(newUrl, index + 1, std::string::npos);
			size_t indexEnd =  newUrl.find_first_of("/");
			_authority._port = newUrl.substr(0, indexEnd);
			newUrl.assign(newUrl, indexEnd, std::string::npos);
		}
		else
		{
			index = newUrl.find_first_of("/");
			_authority._host = newUrl.substr(0, index);
			newUrl.assign(newUrl, index + 1, std::string::npos);
		}
		if (newUrl.find_first_of("?") == std::string::npos && newUrl.find_first_of("#") == std::string::npos)
			_path = newUrl.substr(0, std::string::npos);
		else
		{
			if (newUrl.find_first_of("?") != std::string::npos)
			{
				index = newUrl.find_first_of("?");
				_query = newUrl.substr(0, index);
			}
			if (newUrl.find_first_of("#") != std::string::npos)
			{
				index = newUrl.find_first_of("#");
				_query = newUrl.substr(index, std::string::npos);
			}
		}
	}
	else
	{
		newUrl = url.substr(index + 1);
		_path = newUrl;
	}
}

/* GETTERS */

std::string							&Uri::getUrl()
{ return (_url); }

std::string							&Uri::getScheme()
{ return (_scheme); }

std::string							&Uri::getPath()
{ return (_path); }

std::string							&Uri::getUserInfo()
{ return (_authority._userInfo); }

std::string							&Uri::getHost()
{ return (_authority._host); }

std::string							&Uri::getPort()
{ return (_authority._port); }

std::string							&Uri::getQuery()
{ return (_query); }

std::string							&Uri::getFragment()
{ return (_fragment); }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Uri::Uri()
: _url(""), _scheme(""), _path(""), _query(""), _fragment("")
{ }

Uri::Uri(const Uri &uri)
{ 
	*this = uri;
}

Uri::Uri(const std::string &url)
{
	urlParser(url);
}

Uri::~Uri()
{ }


Uri		&Uri::operator=(const Uri &obj)
{
	_url = obj._url;
	_scheme = obj._scheme;
	_authority._userInfo = obj._authority._userInfo;
	_authority._host = obj._authority._host;
	_authority._port = obj._authority._port;
	_path = obj._path;
	_query = obj._query;
	_fragment = obj._fragment;
	return (*this);
}

/* DEBUG */

void	Uri::debug()
{
	std::cout << "\n***** URI DEBUG *****\n";
	std::cout << "_scheme = " << _scheme << std::endl;
	std::cout << "_userInfo = " << _authority._userInfo << std::endl;
	std::cout << "_host = " << _authority._host << std::endl;
	std::cout << "_port = " << _authority._port << std::endl;
	std::cout << "_path = " << _path << std::endl;
	std::cout << "_query = " << _query << std::endl;
	std::cout << "_fragment = " << _fragment << std::endl;
	std::cout << "***** URI DEBUG END*****\n\n";
}