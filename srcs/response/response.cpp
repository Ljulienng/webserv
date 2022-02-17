#include "response.hpp"

std::vector<struct pollfd*>	g_fileArr;

void    Response::_updateMessage()
{
    // append version
    _message = _httpVersion + " ";

    // append http status
    std::stringstream   stream;
    stream << _httpStatus.getCode();
    _message += stream.str() + " " + _httpStatus.getMessage() + "\r\n";;

    // append headers
    std::map<std::string, std::string>::iterator headerIterator = _headers.begin();
    for ( ; headerIterator != _headers.end(); headerIterator++)
        _message += headerIterator->first + ": " + headerIterator->second + "\r\n";
    _message += "\r\n";
    
    // append content
    _message += std::string(_content.begin(), _content.end());
}

/* SETTERS */
void    Response::setHeader(std::string key, std::string value)
{
    _headers.insert(std::pair<std::string, std::string>(key, value));
}

void    Response::setContent(std::vector<unsigned char> content, std::string contentType)
{
    _content = content;
    setHeader("Content-Type", contentType);
    setHeader("Content-Length", myItoa(content.size()));
}

void    Response::setStatus(int status)
{
    _httpStatus.setStatus(status);
}

/* GETTERS */

/* get all headers */
std::map<std::string, std::string>         &Response::getHeaders()
{ return  _headers; }

/* get one header value thanks to its key */
std::string         Response::getHeader(std::string key)
{
    if (getHeaders().find(key) != getHeaders().end())
        return (getHeaders().find(key)->second);
    return std::string("");
}

std::string     &Response::getHttpVersion()
{ return  _httpVersion; }

HttpStatus      &Response::getHttpStatus()
{ return _httpStatus; }

std::vector<unsigned char>     &Response::getContent()
{ return  _content; }

std::string     &Response::getMessage()
{
    _updateMessage();
    return  _message;
}

/****** test *************/

void 	Response::addFile()
{
    g_fileArr.push_back(&_pollFdFile);
}

void        Response::setPollFd(struct pollfd newPollFd)
{
    _pollFdFile = newPollFd;
}

void        Response::setPollFdFileToRead(const char *file)
{
    _pollFdFile.fd = open(file, O_RDONLY);
    _pollFdFile.events = POLLIN;
    setStateFile(DATATOREAD);
}

void        Response::setStateFile(int state)
{ _stateFile = state; }

struct pollfd       Response::getPollFdFile()
{ return _pollFdFile; }

int   &Response::getStateFile()
{ return _stateFile; }
/****************************/

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Response::Response() : 
        _headers(),
        _httpVersion("HTTP/1.1"),
        _httpStatus(),
        _content(),
        _message(),
        _pollFdFile(),
        _stateFile(NONE)
{
    setHeader("Server", "Webserv_42");
    setHeader("Date", log::getTimestamp());
    setHeader("Connection", "keep-alive");
}

Response::Response(const Response &src)
{
	*this = src;
}

Response::~Response(){}

Response    &Response::operator=(const Response &src)
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