#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"
#include "httpStatus.hpp"

class Response
{
    private :
    // header :
        std::string                     _httpVersion;
        HttpStatus                      _httpStatus;
        std::string                     _contentType;
        size_t                          _contentLength;

    public :
		Response();
		Response(const Response &src);
		~Response();		

        // SETTERS


        // GETTERS
};

#endif