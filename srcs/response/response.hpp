#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"
#include "httpStatus.hpp"
#include "request.hpp"

class Response
{
    private :
    // header :
        std::map<std::string, std::string>  _headers;
        std::string                         _httpVersion;
        HttpStatus                          _httpStatus;
        std::string                         _content;
        std::string                         _message; // the formatted message to insert to the buffer

        void                                _updateMessage();

    public :
		Response();
        Response(Request &request);
		Response(const Response &src);
		~Response();		

        // SETTERS
        void                                setContent(std::string content);

        // GETTERS
        std::map<std::string, std::string>  &getHeaders();
        std::string                         &getHttpVersion();
        HttpStatus                          &getHttpStatus();
        std::string                         &getContent();
        std::string                         &getMessage();
};

#endif