#include "response.hpp"


/* SETTERS */

/* GETTERS */


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