#include "configMatch.hpp"

/*
** find the content in the server
*/

std::string     getUriFolder(std::string uri)
{
    size_t      folderCount = 0;
    std::string newUri;
    std::string uriFolder;


    for (size_t i = 0; uri[i] != 0; i++)
        if (uri[i] == '/')
            folderCount++;
    
    if (folderCount < 2)
        return (uri);
    newUri.assign(uri.substr(1, std::string::npos));
    uriFolder.assign(newUri.substr(0, newUri.find('/')));
    return ("/" + uriFolder);
}

std::string     getPathTranslated(std::string uri, t_configMatch &configMatch)
{
    std::string path = uri;
    std::string pathRoot = getUriFolder(uri);

    // change root if the request match a location block with a root directive
    if (configMatch.location.getPath() == pathRoot && configMatch.location.getRoot().empty() == false)
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
    if (configMatch.location.getAutoindex() == false)
    {
        if ((uri == "/" || path.find_last_of('/') + 1 == path.length()) && configMatch.index.empty() == false)
            path.insert(path.size(), configMatch.index);
        else if (uri == pathRoot && configMatch.location.getIndex().empty() == false)
            path.insert(path.size(), configMatch.location.getIndex());
    }
    std::cerr << "[getPathTranslated] uri = " << uri << "\n";
    std::cerr << "[getPathTranslated] pathRoot = " << pathRoot << "\n";
    std::cerr << "[getPathTranslated] path = " << path << "\n";
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
    // std::cout << "server = " << configMatch.server.getRoot() << std::endl;
    // std::cout << "location = " << configMatch.location.getRoot() << std::endl;
    configMatch.server.getRoot().empty() ? configMatch.root = configMatch.location.getRoot() : configMatch.root = configMatch.server.getRoot();
    configMatch.location.getIndex().empty() ? configMatch.index = configMatch.server.getIndex() : configMatch.index = configMatch.location.getIndex();
    configMatch.pathTranslated = getPathTranslated(request.getUri().getPath(), configMatch); // transform the uri request to match file in the server
    // std::cerr << "configmatch.pathTranslated = " << configMatch.pathTranslated << "\n";
    return configMatch;
}