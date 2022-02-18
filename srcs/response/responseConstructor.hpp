#ifndef RESPONSE_CONSTRUCTOR_HPP
# define RESPONSE_CONSTRUCTOR_HPP

#include "webserv.hpp"
#include "response.hpp"
#include "request.hpp"
#include "configMatch.hpp"
#include "multipart.hpp"

typedef struct  s_configMatch t_configMatch;

Response*       errorResponse(Response* response, t_configMatch  &configMatch, int status);
Response*       cgiResponse(Response* response, std::string cgiResponse, t_configMatch  &configMatch);
Response*       constructResponse(Response* response, Request &request, t_configMatch configMatch);

#endif