#ifndef RESPONSE_CONSTRUCTOR_HPP
# define RESPONSE_CONSTRUCTOR_HPP

#include "webserv.hpp"
#include "response.hpp"
#include "request.hpp"
#include "configMatch.hpp"
#include "multipart.hpp"

Response    errorResponse(Response &response, t_configMatch  &configMatch, int status);
Response    cgiResponse(std::string cgiResponse, Response &response, t_configMatch  &configMatch);
Response    constructResponse(Request &request, std::string serverName);

#endif