#ifndef RESPONSE_CONSTRUCTOR_HPP
# define RESPONSE_CONSTRUCTOR_HPP

#include "webserv.hpp"
#include "response.hpp"
#include "request.hpp"
#include "server.hpp"
#include "location.hpp"


typedef struct  s_configMatch
{
    Server      server;     // which server bloc matches to the request ?
    Location    location;   // which location bloc matches to the request ?
    std::string root;       // which root directory to use [server or location] ?
    std::string index;      // which index to use [server or location] ?
    std::string path;       // the path to search file in the server

}               t_configMatch;


Response    constructResponse(Request &request, std::string serverName);

#endif