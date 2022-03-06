#ifndef CONFIGMATCH_HPP
# define CONFIGMATCH_HPP

#include "webserv.hpp"
#include "file.hpp"
#include "str.hpp"
#include "server.hpp"
#include "configuration.hpp"
#include "location.hpp"
#include "request.hpp"

typedef struct  s_configMatch
{
    Server      server;             // which server bloc matches to the request ?
    Location    location;           // which location bloc matches to the request ?
    std::string root;               // which root directory to use [server or location] ?
    std::string index;              // which index to use [server or location] ?
    std::string pathTranslated;     // path to search file/dir in the server
}               t_configMatch;

std::string     getPathTranslated(std::string uri, t_configMatch &configMatch);
t_configMatch	getConfigMatch(Request &request, std::string serverName);

#endif