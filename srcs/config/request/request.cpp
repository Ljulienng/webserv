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
	_headers["Transfer-Encoding"] = "";
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
	std::string methodsArr[] = {"GET", "POST", "DELETE"};
	std::vector<std::string> methods(methodsArr, methodsArr + sizeof(methodsArr)/sizeof(std::string));

	// TRIM ALL WHITESPACE NEWLINE ETC
	trimChar(_method);
	trimChar(_version);

	// CHECKING VERSION
	if (_version.compare(0, 5, "HTTP/") == 0)
		_version.assign(_version, 5, _version.size() - 2);
	if (_version.compare("1.1"))
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
	arg.assign(line, 0, ++i);
	line.assign(line, i, std::string::npos);
	if (i == std::string::npos)
	{
		std::cerr << "No space after method" << std::endl;
		return ((_ret = 400));
	}
	_method.assign(arg);

	// ASSIGNING PATH
	i = line.find_first_of(' ');
	arg.assign(line, 0, ++i);
	line.assign(line, i, std::string::npos);
	if (i == std::string::npos)
	{
		std::cerr << "No path/HTTP version" << std::endl;
		return ((_ret = 400));
	}
	trimChar(arg);
	_path = arg;

	// ASSIGNING VERSION
	i = line.find_first_of('\n');
	arg.assign(line, 0, --i);
	if (i == std::string::npos)
	{
		std::cerr << "No HTTP version" << std::endl;
		return ((_ret = 400));
	}
	_version.assign(arg);
	_ret = verifArg();
	return (_ret);
}

// Function to iterate on each line of the header
std::string	Request::nextLine(const std::string &request, size_t &i)
{
	std::string	line;
	size_t		j = request.find_first_of("\n", i);

	if (j == std::string::npos)
		return ("\r");
	for (size_t k = i; k < j; k++)
		line.push_back(request[k]);
	// line.assign(request, i, j);
	// Removing carriage return if there is one at the end of the line
	if (line[line.size() - 1] == '\r')
		line.resize(line.size() - 1);
	if (j == std::string::npos)
		i = j;
	else
		i = j + 1;
	return (line);
}

// Function to store values into the header's map of the class using the right keys
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

// The Accept-Language request HTTP header indicates the natural language and locale that the client prefers
void		Request::setAcceptedLanguages()
{
	std::string 				value = _headers["Accept-Language"];
	std::string 				lang;
	std::vector<std::string>	langVec;
	size_t						i;
	float						weight;

	if (value.size() < 5)
		return ;
	langVec = splitString(value, ',');
	for (std::vector<std::string>::iterator it = langVec.begin(); it != langVec.end(); it++)
	{
		weight = 0.0;
		lang.assign((*it), 0, (*it).find_first_of(';'));
		i = (*it).find_last_of('=');
		if (i != std::string::npos)
			weight = atof((*it).substr(i + 1).c_str());
		else
			weight = 1.0;
		_acceptedLang.push_back(std::pair<std::string, float>(lang, weight));
	}
}

void		Request::parseChunkedBody(const std::string &request)
{
	size_t			i = 0;
	std::string		chunks = request.substr(request.find("\r\n\r\n") + 4, request.find("0\r\n\r\n") + 5);
	long			chunkSize = strtol(chunks.substr(i, chunks.find("\r\n", i) + 2).c_str(), NULL, 16);

	while (chunkSize > 0)
	{
		i = chunks.find("\r\n", i) + 2;
		_body.append(i, chunkSize);
		i += chunkSize + 2;
		chunkSize = strtol(chunks.substr(i, chunks.find("\r\n", i) + 2).c_str(), NULL, 16);
	}
}

void		Request::parsebody(const std::string &request)
{
	size_t j = 0;

	// Looking for the line that separates the header and the body of the http request
	j = request.find("\r\n\r\n");
	if (_headers["Content-Length"] != "")
		_body.assign(request, j + 4, atoi(_headers["Content-Length"].c_str()));
}

void		Request::initUri()
{
	_uri.createUrl(_headers, _path);
	_uri.urlParser(_uri.getUrl());
	_uri.debug();
}

int			Request::parse(const std::string &request)
{
	std::string line;
	std::string tmp;
	size_t 		i = 0;

	if (!request.size())
	{
		std::cerr << "Request is empty" << std::endl;
		return (400);
	}
	initHeaders();
	i = request.find_first_of('\n');
	line = request.substr(0, i);
	// Store the first line to get the Method, Path and Version
	_ret = parseFirstLine(line);
	tmp.assign(request, i, std::string::npos);
	i = tmp.find_first_of('\n') + 1;
	// Parsing each line of the header assigning values to the corresponding keys
	parseHeader(tmp, i);
	setAcceptedLanguages();
	initUri();
	if (tmp.size())
		tmp.assign(request, i, std::string::npos);
	if (_headers["Transfer-Encoding"] == "chunked")
		parseChunkedBody(tmp);
	else
		parsebody(tmp);
	// debug();
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

/* get one header value thanks to its key */
std::string         Request::getHeader(std::string key)
{
    if (getHeaders().find(key) != getHeaders().end())
        return (getHeaders().find(key)->second);
    return std::string("");
}

std::string							&Request::getBody()
{ return (_body); }

Uri									&Request::getUri()
{ return (_uri); }

int									Request::getRet()
{ return (_ret); }


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
	_method(""), _path(""), _version(""), _headers(), _body(""), _ret(200)
{
	parse(request);
}

Request::Request(const Request &obj) :
	_method(obj._method), _path(obj._path), _version(obj._version), _headers(obj._headers), _body(obj._body), _ret(obj._ret)
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
	return (*this);
}

/* DEBUG */

void			Request::debug()
{
	std::map<std::string, std::string>::iterator 			it;
	std::vector<std::pair<std::string, float> >::iterator	it2;

	std::cout << "\n***** DEBUG *****\n";
	std::cout << "_METHOD = " << _method << std::endl;
	// _url.debug();
	std::cout << "_version = " << _version << std::endl;
	std::cout << "_ret = " << _ret << std::endl;

	std::cout << "\n***** HEADERS KEY = VALUE *****\n";
	for (it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << " = " << it->second << std::endl;
	std::cout << "\n***** ACCEPTED LANG *****\n";
	for (it2 = _acceptedLang.begin(); it2 != _acceptedLang.end(); it2++)
		std::cout << (*it2).first << " = " << (*it2).second << std::endl;
	std::cout << "\n***** BODY *****\n";
	std::cout << _body;
	std::cout << "\n***** END OF DEBUG *****\n";
}