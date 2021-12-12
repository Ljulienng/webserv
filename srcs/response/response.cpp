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



/* GETTERS */

std::map<std::string, std::string>         &Response::getHeaders()
{
    return  _headers;
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