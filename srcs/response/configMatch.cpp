#include "configMatch.hpp"

std::string     getServerPath(std::string uri, t_configMatch &configMatch)
{
    std::string path;

    // if uri is file ->
    //          - if exist : display content
    //          - else : 404 error
    // if uri is folder ->
    //          - if doesn't exist : 404 error
    //          - else :
    //                    - autoindex on : display list of file
    //                    - autoindex off : 
    //                                      - if default file : display
    //                                      - else : 404 error

    path = configMatch.root + uri;

    return path;
}

/*
** get the context of the request
*/
t_configMatch	getConfigMatch(Request &request, std::string serverName)
{
    t_configMatch   configMatch;

    configMatch.server = Configuration::getInstance().findServer(serverName);
    configMatch.location = configMatch.server.findLocation(request.getPath());
    configMatch.location.getRoot().empty() ? configMatch.root = configMatch.server.getRoot() : configMatch.root = configMatch.location.getRoot();
    configMatch.location.getIndex().empty() ? configMatch.index = configMatch.server.getIndex() : configMatch.index = configMatch.location.getIndex();
    configMatch.path = getServerPath(request.getUri().getPath(), configMatch); // transform the uri request to match in the server

    return configMatch;
}