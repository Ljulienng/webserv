#include "response.hpp"

void            Response::_updateMessage()
{
    // append version
    _message = _httpVersion + " ";

    // append http status
    std::stringstream   ss;
    ss << _httpStatus.getCode();
    _message += ss.str() + " " + _httpStatus.getMessage() + "\r\n";;

    // append headers
    std::map<std::string, std::string>::iterator headerIterator = _headers.begin();
    for ( ; headerIterator != _headers.end(); headerIterator++)
        _message += headerIterator->first + ": " + headerIterator->second + "\r\n";
    _message += "\r\n";
    
    // append content
    _message += _content;
}

/* SETTERS */

void        Response::setContent(std::string content)
{
    _content = content;
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
        _httpStatus(),
        _content(),
        _message()
			// to be completed if new attributes
{}

Response::Response(Request &request) : 
        _headers(),
        _httpVersion(request.getVersion()),
        _httpStatus(),
        _content(),
        _message()
{}

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