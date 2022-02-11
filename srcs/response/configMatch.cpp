#include "configMatch.hpp"

/*
** find the content in the server
*/
std::string     getPathTranslated(std::string uri, t_configMatch &configMatch)
{
    std::string path = uri;
   
    // change root if the request match a location block with a root directive
    if (configMatch.location.getPath() == uri && configMatch.location.getRoot().empty() == false)
    {
        path.erase(0, configMatch.location.getPath().size());
        if (path.empty() == true || path[0] != '/')
            path.insert(0, "/");
        path.insert(0, configMatch.location.getRoot());
    }
    else // default root directive
    {
        if (path.empty() == true || path[0] != '/')
            path.insert(0, "/");
        path.insert(0, configMatch.root);
    }
    
    // add index file if we have one and if no autoindex directive
    if (configMatch.location.getAutoindex() == false && configMatch.location.getIndex().empty() == false)
        path.insert(path.size(), configMatch.location.getIndex());

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
    configMatch.pathTranslated = getPathTranslated(request.getUri().getPath(), configMatch); // transform the uri request to match file in the server
    std::cerr << "configmatch.pathTranslated = " << configMatch.pathTranslated << "\n";
    return configMatch;
}