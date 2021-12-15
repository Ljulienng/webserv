#include "response.hpp"
#include "utils.hpp"
void            Response::_updateMessage()
{
    // append version
    _message = _httpVersion + " ";

    // append http status
    std::stringstream   stream;
    stream << _httpStatus.getCode();
    _message += stream.str() + " " + _httpStatus.getMessage() + "\r\n";;

    // // append headers
    // std::map<std::string, std::string>::iterator headerIterator = _headers.begin();
    // for ( ; headerIterator != _headers.end(); headerIterator++)
    //     _message += headerIterator->first + ": " + headerIterator->second + "\r\n";
    // _message += "\r\n";
    
    // // append content
    // _message += _content;

    // TEST minimum content-->>   
    _message += "Connection: keep-alive\r\n";
    _message += "Content-length: 49\r\n";
    _message += "Content-Type: text/html; charset=UTF-8\r\n";
    _message += "Date: \r\n";
    _message += "Server: Webserv\r\n";
    _message += "\r\n";
    _message += "<html><body><h1>Hello world !</h1></body></html>";
}

/* SETTERS */

void        Response::setHeader(std::string key, std::string value)
{
    _headers.insert(std::pair<std::string, std::string>(key, value));
}

void        Response::setContent(std::string content, std::string contentType)
{
    _content = content;
    setHeader("Content-Type", contentType);
    setHeader("Content-Length", utils::myItoa(content.size()));
}

void        Response::setStatus(HttpStatus status)
{
    _httpStatus = status;
}

/* GETTERS */

/* get all headers */
std::map<std::string, std::string>         &Response::getHeaders()
{
    return  _headers;
}

/* get one header value thanks to its key */
std::string         Response::getHeader(std::string key)
{
    if (this->getHeaders().find(key) != this->getHeaders().end())
        return (this->getHeaders().find(key)->second);
    return (NULL);
}

std::string         &Response::getHttpVersion()
{
    return  _httpVersion;
}

HttpStatus          &Response::getHttpStatus()
{
    return _httpStatus;
}

std::string         &Response::getContent()
{
    return  _content;
}

std::string         &Response::getMessage()
{
    _updateMessage();
    return  _message;
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Response::Response() :
        _headers(),
        _httpVersion(),
        _httpStatus(200),
        _content(),
        _message()
			// to be completed if new attributes
{}


Response::Response(Request &request, Configuration &config, std::string serverName) : 
        _headers(),
        _httpVersion("HTTP/1.1"),
        _httpStatus(200),
        _content(),
        _message()
{
    setHeader("Server", "Webserv_42");
    setHeader("Connection", "keep-alive");
    setHeader("Date", utils::getTimestamp());
    // build the response thanks to the request
    (void)request;

    // first need to get the server and location to use for this response (context)
    Server &server = config.findServer(serverName);
    (void)server;
    // need to get the uri store in the request
    // get the location bloc concerned
    // get the method -> if no method -> set status and print error
    // choose the execution beetween : 
    //      - need cgi ?  if yes -> exec cgi
    //      - redirection
    //      - method get, post or delete
}

Response::Response(const Response &src)
{
	*this = src;
}

Response::~Response() {}

Response &Response::operator=(const Response &src)
{
	if (&src != this)
	{
		_headers = src._headers;
        _httpVersion = src._httpVersion;
        _httpStatus = src._httpStatus;
        _content = src._content;
        _message = src._message;
		// to be completed if new attributes
	}
	return (*this);
}