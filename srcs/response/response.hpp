#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"
#include "httpStatus.hpp"
#include "request.hpp"
#include "html.hpp"
#include "utils.hpp"

class Configuration;
class Server;
class Location;

class Response
{
    private :
        std::map<std::string, std::string>  _headers;
        std::string                         _httpVersion;
        HttpStatus                          _httpStatus;
        std::string                         _content;
        std::string                         _message; // the formatted message to insert to the buffer

        void        _updateMessage();
        void        _dispatchingResponse(Request &request, Server &server, Location &location, std::string index, std::string root);
        void        _cgiResponse();
        void        _getMethodResponse(Location &location, std::string _path, std::string index, std::string root);
        void        _postMethodResponse();
        void        _deleteMethodResponse(std::string path, std::string root);
        void        _buildAutoIndexResponse(std::string path);
        void        _buildIndexResponse(std::string path, std::string index);
        void        _redirectionResponse(std::pair<int, std::string> redirection);
        void        _buildErrorResponse(std::string root, int status);

    public :
		Response();
        Response(Request &request, std::string serverName);
		Response(const Response &src);
		~Response();
        Response &operator=(const Response &src);	

        // SETTERS
        void                                setHeader(std::string key, std::string value);
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