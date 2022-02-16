#ifndef MULTIPART_HPP
# define MULTIPART_HPP

#include "webserv.hpp"
// #include "request.hpp"
#include "response.hpp"
#include "configMatch.hpp"
#include "responseConstructor.hpp"

typedef struct  s_multipart
{
    std::map<std::string, std::string>  headers;
    unsigned char                       *content;
    size_t                              length;
    std::string                         getFilename() const;
}               t_multipart;

Response    multipart(Response &response, Request &request, t_configMatch &configMatch, std::string contentTypeHeader);

#endif