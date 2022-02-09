#ifndef RESPONSE_CONSTRUCTOR_HPP
# define RESPONSE_CONSTRUCTOR_HPP

#include "webserv.hpp"
#include "response.hpp"
#include "request.hpp"
#include "configMatch.hpp"

class cgiConstructor;

typedef struct  s_multipart
{
    std::map<std::string, std::string>  headers;
    unsigned char                       *content;
    size_t                              length;
    std::string     getFilename() const;
}               t_multipart;

Response    cgiResponse(std::string cgiResponse, Response &response, t_configMatch  &configMatch);
Response    constructResponse(Request &request, std::string serverName);

#endif