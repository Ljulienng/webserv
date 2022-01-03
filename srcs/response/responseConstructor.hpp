#ifndef RESPONSE_CONSTRUCTOR_HPP
# define RESPONSE_CONSTRUCTOR_HPP

#include "webserv.hpp"
#include "response.hpp"
#include "request.hpp"
#include "server.hpp"
#include "location.hpp"

typedef struct  s_configMatch
{
    Server      server;
    Location    location;
    std::string root;
    std::string index;

}               t_configMatch;


std::string     parseUrl(Server &server, Location &location, std::string url, std::string index, std::string root);
std::string     getExtension(std::string filename);
Response    errorResponse(Response &response, std::string root, int status);
Response    autoIndexResponse(Response &response, std::string path);
Response    indexResponse(Response &response,std::string path, std::string index);
Response    cgiResponse(Response &response);
Response    redirectionResponse(Response &response, std::pair<int, std::string> redirection);
Response    getMethodResponse(Response &response, Location &location, std::string _path, std::string index, std::string root);
Response    postMethodResponse(Response &response);
Response    deleteMethodResponse(Response &response, std::string path, std::string root);
Response    dispatchingResponse(Response &response, Request &request, Server &server, Location &location, std::string index, std::string root);
Response    constructResponse(Request &request, std::string serverName);

#endif