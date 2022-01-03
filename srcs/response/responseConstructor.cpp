
#include "response.hpp"
#include "request.hpp"
#include "server.hpp"
#include "location.hpp"

std::string     parseUrl(Server &server, Location &location, std::string url, std::string index, std::string root)
{
    std::string newUrl;
    (void)index;
    (void)server;
    (void)location;

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
    newUrl = root + url;
    return newUrl;
}

std::string     getExtension(std::string filename)
{
    size_t index = filename.find_last_of(".") + 1;
    if (index == std::string::npos)
        return NULL;
    return (filename.substr(index, std::string::npos));
}

void    _buildErrorResponse(std::string root, int status)
{
    std::string     errorPage = Configuration::getInstance().getErrorPages()[status];
    File            errorPath(root + errorPage);

    _httpStatus.setStatus(status);
    if (!errorPage.empty() && errorPath.isRegularFile())
    {
        setContent(errorPath.getFileContent(), "text/html");
    }
    else
    {
        std::string defaultErrorPage = html::buildErrorHtmlPage(utils::myItoa(status));
        setContent(defaultErrorPage, "text/html");
    }
}

Response    _buildAutoIndexResponse(Response &response, std::string path)
{
    std::string autoIndexPage;

    response.getHttpStatus().setStatus(200);
    autoIndexPage = html::buildAutoIndexPage(path);
    response.setContent(autoIndexPage, "text/html");
    
    return response;
}

Response    _buildIndexResponse(Response &response,std::string path, std::string index)
{
    File indexFile(path + index);
    Mime indexExtension(getExtension(index));

    response.getHttpStatus().setStatus(200);
    response.setContent(indexFile.getFileContent(), indexExtension.getMime()); // set content-type + content-length + content
    
    return response;
}

Response    _cgiResponse(Response &response)
{
    std::cout << "Need to execute CGI\n";
    return response;
}

Response    _redirectionResponse(Response &response, std::pair<int, std::string> redirection)
{
    std::string redirectionPage;

    std::cout << "Redirection " << redirection.first << " -> " << redirection.second << "\n";
    response.getHttpStatus().setStatus(redirection.first);
    response.setHeader("Location", redirection.second);
    redirectionPage = html::buildRedirectionPage(redirection);
    response.setContent(redirectionPage, "text/html");

    return response;
}

Response    _getMethodResponse(Response &response, Location &location, std::string _path, std::string index, std::string root)
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
    else if (path.isDirectory() && location.getAutoindex())
    {
        std::cout << "Directory -> autoindex\n";
        return _buildAutoIndexResponse(_path);
    }
    else if (path.isDirectory() && !location.getAutoindex() && !index.empty() && (location.getPath() == "/"))
    {
        std::cout << "Directory -> index\n";
        return _buildIndexResponse(_path, index);

    }
    else // not found
    {
        std::cout << "Error not found\n";
        return _buildErrorResponse(root, 404); // send error response and page 404.html
    }
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
Response    _dispatchingResponse(Response &response, Request &request, Server &server, Location &location, std::string index, std::string root)
{
    // then we need to transform the uri request to match in the server
    std::string path = parseUrl(server, location, request.getPath(), index, root); //TO IMPLEMENT

    if (request.getPath().find(Configuration::getInstance().getCgi().first) != std::string::npos
        && (request.getMethod() == "GET" || request.getMethod() == "POST"))
        return _cgiResponse(response);
    else if (location.getRedirection().first > 0 && !location.getRedirection().second.empty())
        return _redirectionResponse(response, location.getRedirection());
    else if (request.getMethod() == "GET")
        return _getMethodResponse(response, location, path, index, root);
    else if (request.getMethod() == "POST")
        return_postMethodResponse(response);
    else if (request.getMethod() == "DELETE")
        return _deleteMethodResponse(response, path, root);
    return _buildErrorResponse(root, 404);
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