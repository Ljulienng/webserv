#include "responseConstructor.hpp"

std::string     parseUrl(std::string url, t_configMatch &configMatch)
{
    std::string newUrl;

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

    // test
    // File    indexFile(root + url + index);
    
    // if (url[url.size() - 1] == '/' && indexFile.isRegularFile())    // if directory and directory/index exist
    //     newUrl = root + url + index;
    // else                                                         // file or directory without autoindex
    //     newUrl = root + url;
    newUrl = configMatch.root + url;
    return newUrl;
}

std::string     getExtension(std::string filename)
{
    size_t index = filename.find_last_of(".") + 1;
    if (index == std::string::npos)
        return NULL;
    return (filename.substr(index, std::string::npos));
}

Response    errorResponse(Response &response, t_configMatch  &configMatch, int status)
{
    std::string     errorPage = Configuration::getInstance().getErrorPages()[status];
    File            errorPath(configMatch.root + errorPage);

    response.getHttpStatus().setStatus(status);
    if (!errorPage.empty() && errorPath.isRegularFile())
    {
        response.setContent(errorPath.getFileContent(), "text/html");
    }
    else
    {
        std::string defaultErrorPage = html::buildErrorHtmlPage(utils::myItoa(status));
        response.setContent(defaultErrorPage, "text/html");
    }

    return response;
}

Response    autoIndexResponse(Response &response, std::string path)
{
    std::string autoIndexPage;

    response.getHttpStatus().setStatus(200);
    autoIndexPage = html::buildAutoIndexPage(path);
    response.setContent(autoIndexPage, "text/html");
    
    return response;
}

Response    indexResponse(Response &response,std::string path, std::string index)
{
    File indexFile(path + index);
    Mime indexExtension(getExtension(index));

    response.getHttpStatus().setStatus(200);
    response.setContent(indexFile.getFileContent(), indexExtension.getMime()); // set content-type + content-length + content
    
    return response;
}

Response    cgiResponse(Response &response, t_configMatch  &configMatch)
{
    (void)configMatch;
    std::cout << "Need to execute CGI\n";
    return response;
}

Response    redirectionResponse(Response &response, std::pair<int, std::string> redirection)
{
    std::string redirectionPage;

    std::cout << "Redirection " << redirection.first << " -> " << redirection.second << "\n";
    response.getHttpStatus().setStatus(redirection.first);
    response.setHeader("Location", redirection.second);
    redirectionPage = html::buildRedirectionPage(redirection);
    response.setContent(redirectionPage, "text/html");

    return response;
}

Response    getMethodResponse(Response &response, std::string _path, t_configMatch &configMatch)
{
    File        path(_path);
    
    if (path.isRegularFile())
    {
        std::cout << "File -> ok regular file\n";
        Mime    extension(getExtension(_path));

        response.getHttpStatus().setStatus(200);
        response.setContent(path.getFileContent(), extension.getMime()); // set content-type + content-length + content
        return response;
    }
    else if (path.isDirectory() && configMatch.location.getAutoindex())
    {
        std::cout << "Directory -> autoindex\n";
        return autoIndexResponse(response, _path);
    }
    else if (path.isDirectory() && !configMatch.location.getAutoindex() && !configMatch.index.empty() && (configMatch.location.getPath() == "/"))
    {
        std::cout << "Directory -> index\n";
        return indexResponse(response, _path, configMatch.index);

    }
    else // not found
    {
        std::cout << "Error not found\n";
        return errorResponse(response, configMatch, 404); // send error response and page 404.html
    }
}

Response    postMethodResponse(Response &response, t_configMatch &configMatch)
{
    (void)configMatch;
    return response;
}

Response    deleteMethodResponse(Response &response, std::string path, t_configMatch &configMatch)
{
    File    fileToDelete(path);

    if (fileToDelete.isRegularFile())
    {
       if (remove(path.c_str()) == 0)
        {
            response.getHttpStatus().setStatus(200);
            response.setContent(html::buildPage("Method DELETE ok : file successfully deleted"), "text/html");
            return response;
        }
        else
            return errorResponse(response, configMatch, 204);
    }
    else
        return errorResponse(response, configMatch, 204);
}

/*
** evaluate the type of the response we need :
**      - get
**      - post
**      - delete
**      - cgi
**      - redirection
**      - error
*/
Response    dispatchingResponse(Response &response, Request &request, t_configMatch  &configMatch)
{
    // then we need to transform the uri request to match in the server
    std::string path = parseUrl(request.getPath(), configMatch); //TO IMPLEMENT

    if (request.getPath().find(Configuration::getInstance().getCgi().first) != std::string::npos
        && (request.getMethod() == "GET" || request.getMethod() == "POST"))
        return cgiResponse(response, configMatch);
    else if (configMatch.location.getRedirection().first > 0 && !configMatch.location.getRedirection().second.empty())
        return redirectionResponse(response, configMatch.location.getRedirection());
    else if (request.getMethod() == "GET")
        return getMethodResponse(response, path, configMatch);
    else if (request.getMethod() == "POST")
        return postMethodResponse(response, configMatch);
    else if (request.getMethod() == "DELETE")
        return deleteMethodResponse(response, path, configMatch);
    return errorResponse(response, configMatch, 404);
}

Response    constructResponse(Request &request, std::string serverName)
{
    Response    response;
    std::string root;
    std::string index;

    // first need to get the server and location to use for this response (context)
    t_configMatch   configMatch;
    configMatch.server = Configuration::getInstance().findServer(serverName);
    configMatch.location = configMatch.server.findLocation(request.getPath());
    configMatch.location.getRoot().empty() ? configMatch.root = configMatch.server.getRoot() : configMatch.root = configMatch.location.getRoot();
    configMatch.location.getIndex().empty() ? configMatch.index = configMatch.server.getIndex() : configMatch.index = configMatch.location.getIndex();
    
    // create a class with the server, location, root, index and all context matching the request
    response = dispatchingResponse(response, request, configMatch);

    return response;
}