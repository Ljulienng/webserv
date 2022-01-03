
#include "response.hpp"
#include "request.hpp"
#include "server.hpp"
#include "location.hpp"

/*
** evaluate the type of the response we need :
**      - get
**      - post
**      - delete
**      - cgi
**      - redirection
**      - error
*/
Response    _dispatchingResponse(Response &response, Request &request, Server &server, Location &location, std::string index, std::string root)
{
    // then we need to transform the uri request to match in the server
    std::string path = parseUrl(server, location, request.getPath(), index, root); //TO IMPLEMENT

    if (request.getPath().find(Configuration::getInstance().getCgi().first) != std::string::npos
        && (request.getMethod() == "GET" || request.getMethod() == "POST"))
        _cgiResponse();
    else if (location.getRedirection().first > 0 && !location.getRedirection().second.empty())
        _redirectionResponse(location.getRedirection());
    else if (request.getMethod() == "GET")
        _getMethodResponse(location, path, index, root);
    else if (request.getMethod() == "POST")
        _postMethodResponse();
    else if (request.getMethod() == "DELETE")
        _deleteMethodResponse(path, root);
}

Response    constructResponse(Request &request, std::string serverName)
{
    Response    response;
    std::string root;
    std::string index;

    // first need to get the server and location to use for this response (context)
    Server &server = Configuration::getInstance().findServer(serverName);
    Location &location = server.findLocation(request.getPath());
    location.getRoot().empty() ? root = server.getRoot() : root = location.getRoot();
    location.getIndex().empty() ? index = server.getIndex() : index = location.getIndex();
    
    // create a class with the server, location, root, index and all context matching the request
    response = _dispatchingResponse(response, request, server, location, index, root);

    return response;
}