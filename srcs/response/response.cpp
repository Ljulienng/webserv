#include "response.hpp"


/* SETTERS */

/* GETTERS */

std::string         &Response::getHttpVersion()
{
    return  _httpVersion;
}

HttpStatus          &Response::getHttpStatus()
{
    return _httpStatus;
}

std::string         &Response::getContentType()
{
    return  _contentType;
}

size_t              &Response::getContentLength()
{
    return  _contentLength;
}


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Response::Response() :
        _httpVersion(),
        _httpStatus(),
        _contentType(),
        _contentLength(0)
			// to be completed if new attributes
{
    (void)_contentLength;
}

Response::Response(const Response &src)
{
	*this = src;
}

Response::~Response() {}