#include "request.hpp"


/* PARSING */

void	Request::initHeaders()
{
	_headers.clear();

	// Authentification 
	_headers["Authorization"] = "";
	_headers["WWW-Authenticate"] = "";

	// Caching
	_headers["Age"] = "";
	_headers["Cache-Control"] = "";
	_headers["Expires"] = "";

	// Conditionnal
	_headers["Last-Modified"] = "";

	// Connexion management
	_headers["Connection"] = "";

	// Content negociation
	_headers["Accept"] = "";
	_headers["Accept-Charset"] = "";
	_headers["Accept-Encoding"] = "";
	_headers["Accept-Language"] = "";

	// Information for the body message
	_headers["Content-Length"] = "";
	_headers["Content-Type"] = "";
	_headers["Content-Encoding"] = "";
	_headers["Content-Language"] = "";

	// Redirection
	_headers["Location"] = "";

	// Query content
	_headers["Host"] = "";
	_headers["Referer"] = "";
	_headers["Referrer-Policy"] = "";

	// Response context
	_headers["Allow"] = "";
	_headers["Server"] = "";

	// Others
	_headers["Date"] = "";
}

int		Request::verifArg()
{
	// Initializing all valid methods into a vector
	std::string methodsArr[] = {"GET", "HEAD", "POST", "PUT", "DELETE", "OPTIONS", "TRACE"};
	std::vector<std::string> methods(methodsArr, methodsArr + sizeof(methodsArr)/sizeof(std::string));

	// TRIM ALL WHITESPACE ETC
	trimChar(_method);
	trimChar(_path);
	trimChar(_version);

	// CHECKING VERSION
	if (_version.compare(0, 5, "HTTP/") == 0)
		_version.assign(_version, 5, std::string::npos - 1);
	if (_version != "1.1" && _version != "1.0")
	{
		std::cerr << "Bad HTTP version" << std::endl;
		return ((_ret = 400));
	}	
	
	//CHECKING METHOD
	for (size_t i = 0; i < methods.size(); i++)
		if (_method == methods[i])
			return (_ret);
	std::cerr << "Invalid method" << std::endl;
	return ((_ret = 400));
}

int		Request::parseFirstLine(std::string line)
{
	size_t i;
	std::string arg;


	// ASSIGNING METHOD
	i = line.find_first_of(' ');
	i++;
	arg.assign(line, 0, i);
	line.assign(line, i, std::string::npos);
	if (i == std::string::npos)
	{
		std::cerr << "No space after method" << std::endl;
		return ((_ret = 400));
	}
	_method.assign(arg);
	arg.clear();

	// ASSIGNING PATH
	i = line.find_first_of(' ');
	i++;
	arg.assign(line, 0, i);
	line.assign(line, i, std::string::npos);
	if (i == std::string::npos)
	{
		std::cerr << "No path/HTTP version" << std::endl;
		return ((_ret = 400));
	}
	_path.assign(arg);
	arg.clear();

	// ASSIGNING VERSION
	i = line.find_first_of('\n');
	i--;
	arg.assign(line, 0, i);
	if (i == std::string::npos)
	{
		std::cerr << "No HTTP version" << std::endl;
		return ((_ret = 400));
	}
	_version.assign(arg);
	arg.clear();
	_ret = verifArg();
	return (_ret);
}

std::string	Request::nextLine(const std::string &request, size_t &i)
{
	std::string	line;
	size_t		j = request.find_first_of("\n", i);

	for (size_t k = i; k < j; k++)
		line.push_back(request[k]);
	// line.assign(request, i, j);
	if (line[line.size() - 1] == '\r')
	{
		line.resize(line.size() - 1);
	}

	if (j == std::string::npos)
		i = j;
	else
		i = j + 1;
	return (line);
}

void		Request::storeKeyValue(const std::string &line)
{
	std::string key;
	std::string value;
	size_t i;

	i = line.find_first_of(':');
	key.assign(line, 0, i);
	// debug();
	capitalizeString(key);
	i = line.find_first_not_of(' ', i + 1);
	if (i != std::string::npos)
		value.assign(line, i, std::string::npos);
	// Removing all trailing spaces
	key.erase(remove(key.begin(), key.end(), ' '), key.end());
	value.erase(remove(value.begin(), value.end(), ' '), value.end());

	std::cout << "key = " << key << " value = " << value << std::endl;
	// Assigning value to the right key
	if (_headers.count(key))
		_headers[key] = value;
}

int			Request::parseHeader(const std::string &request, size_t &i)
{

	std::string line;

	line = nextLine(request, i);
	while (request.size() != 0 && _ret != 400 && line != "\r" && line.size())
	{
		storeKeyValue(line);
		line = nextLine(request, i);
	}
	return (_ret);
}

void		Request::parsebody(const std::string &request)
{
	_body.assign(request);
}

int			Request::parse(const std::string &request)
{
	std::string line;
	std::string tmp;
	size_t 		i = 0;

	initHeaders();
	i = request.find_first_of('\n');
	line = request.substr(0, i);
	_ret = parseFirstLine(line);
	tmp.assign(request, i, std::string::npos);
	i = tmp.find_first_of('\n') + 1;
	parseHeader(tmp, i);
	debug();
	return (_ret);
}

/* GETTERS */

std::string							&Request::getMethod()
{ return (_method); }

std::string							&Request::getPath()
{ return (_path); }

std::string							&Request::getVersion()
{ return (_version); }

std::map<std::string, std::string>	&Request::getHeaders()
{ return (_headers); }

std::string							&Request::getBody()
{ return (_body); }

int									Request::getPort()
{ return (_port); }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

// Request::Request() :
// 	_rawRequest(""), _method(""), _version(""), _headers(""), _body(""), _port(80), _ret(0)
// {
// }	

/* MEMO
200 OK
201 Created
202 Accepted
203 Non-Authoritative Information (since HTTP/1.1)
204 No Content
205 Reset Content
*/

Request::Request(const std::string &request) :
	_method(""), _version(""), _headers(), _body(""), _port(8080), _ret(0)
{
	parse(request);
}

Request::Request(const Request &obj) :
	_method(obj._method), _version(obj._version), _headers(obj._headers), _body(obj._body), _port(8080), _ret(obj._ret)
{}

Request::~Request()
{}

Request			&Request::operator=(const Request &obj)
{
	_method = obj._method;
	_path = obj._path;
	_version = obj._version;
	_headers = obj._headers;
	_body = obj._body;
	_port = obj._port;
	return (*this);
}

/* DEBUG */

void			Request::debug()
{
	std::map<std::string, std::string>::iterator it;

	std::cout << "\n***** DEBUG *****\n";
	std::cout << "_METHOD = " << _method << std::endl;
	std::cout << "_PATH = " << _path << std::endl;
	std::cout << "_version = " << _version << std::endl;
	std::cout << "_body = " << _body << std::endl;
	std::cout << "_port = " << _port << std::endl;
	std::cout << "_ret = " << _ret << std::endl;

	std::cout << "\n***** HEADERS KEY = VALUE *****\n";
	for (it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << " = " << it->second << std::endl;
	std::cout << "\n***** END OF DEBUG *****\n";
	
}