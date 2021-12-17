#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"
#include "httpStatus.hpp"
#include "request.hpp"
#include "utils.hpp"

class Configuration;
class Server;
class Location;

class Response
{
    private :
    // header :
        std::map<std::string, std::string>  _headers;
        std::string                         _httpVersion;
        HttpStatus                          _httpStatus;
        std::string                         _content;
        std::string                         _message; // the formatted message to insert to the buffer

        void        _updateMessage();
        void        _dispatchingResponse(Request &request, Server &server, Location &location);
        void        _getResponse();
        void        _postResponse();
        void        _deleteResponse();

    public :
		Response();
        Response(Request &request, Configuration &config, std::string serverName);
		Response(const Response &src);
		~Response();
        Response &operator=(const Response &src);	

        // SETTERS
        void                                setHeader(std::string key, std::string value);
        void                                setStatus(HttpStatus status);
        void                                setContent(std::string content, std::string contentType);

        // GETTERS
        std::map<std::string, std::string>  &getHeaders();
        std::string                         getHeader(std::string key);
        std::string                         &getHttpVersion();
        HttpStatus                          &getHttpStatus();
        std::string                         &getContent();
        std::string                         &getMessage();
};

#include "configuration.hpp"

#endif