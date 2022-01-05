#include "responseConstructor.hpp"

std::string     getRelativePath(std::string path)
{
    (void)path;
    return path;
}

std::string     parseUri(std::string uri, t_configMatch &configMatch)
{
    std::string newUri;
    //std::string relativePath = getRelativePath(uri);

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
    newUri = configMatch.root + uri;
    return newUri;
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

    response.setStatus(200);
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
    response.setStatus(redirection.first);
    response.setHeader("Location", redirection.second);
    redirectionPage = html::buildRedirectionPage(redirection);
    response.setContent(redirectionPage, "text/html");

    return response;
}

Response    getMethodResponse(Response &response, t_configMatch &configMatch)
{
    File        path(configMatch.path);
    
    if (path.isRegularFile())
    {
        std::cout << "File -> ok regular file\n";
        Mime    extension(getExtension(configMatch.path));

        response.setStatus(200);
        response.setContent(path.getFileContent(), extension.getMime()); // set content-type + content-length + content
        return response;
    }
    else if (path.isDirectory() && configMatch.location.getAutoindex())
    {
        std::cout << "Directory -> autoindex\n";
        return autoIndexResponse(response, configMatch.path);
    }
    else if (path.isDirectory() && !configMatch.location.getAutoindex() && !configMatch.index.empty() && (configMatch.location.getPath() == "/"))
    {
        std::cout << "Directory -> index\n";
        return indexResponse(response, configMatch.path, configMatch.index);

    }
    else
    {
        std::cout << "Error not found\n";
        return errorResponse(response, configMatch, 404); // send error response and page 404.html
    }
}

Response    postMethodResponse(Response &response, Request &request, t_configMatch &configMatch)
{
    File            fileToPost(configMatch.path);
    std::string     pathToUpload;

    // check we have a directory to uploads files else errorResponse
    if (configMatch.server.getUploadPath() == "")
        return errorResponse(response, configMatch, 403);

    pathToUpload = configMatch.root + configMatch.server.getUploadPath();

    // we create the file
    if (!fileToPost.createFile(pathToUpload, request.getBody()))
        return errorResponse(response, configMatch, 500);
    response.setStatus(201);

    // we indicate the url of the resource we created thanks to "location" header
    response.setHeader("Location", request.getPath());
    html::buildRedirectionPage(std::pair<int, std::string>(201, request.getPath()));
    
    return response;
}

Response    deleteMethodResponse(Response &response, t_configMatch &configMatch)
{
    File    fileToDelete(configMatch.path);

    if (fileToDelete.exists())
    {
       if (remove(configMatch.path.c_str()) == 0)
        {
            response.setStatus(200);
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
    File        path(configMatch.path);

    if (path.isDirectory() && configMatch.path[configMatch.path.size() - 1] != '/')
        return redirectionResponse(response, std::pair<int, std::string>(301, request.getPath() + "/"));
    else if (Configuration::getInstance().getCgi().first == ".php"
        && request.getPath().find(Configuration::getInstance().getCgi().first) != std::string::npos
        && (request.getMethod() == "GET" || request.getMethod() == "POST"))
        return cgiResponse(response, configMatch);
    else if (configMatch.location.getRedirection().first > 0 && !configMatch.location.getRedirection().second.empty())
        return redirectionResponse(response, configMatch.location.getRedirection());
    else if (request.getMethod() == "GET")
        return getMethodResponse(response, configMatch);
    else if (request.getMethod() == "POST")
        return postMethodResponse(response, request, configMatch);
    else if (request.getMethod() == "DELETE")
        return deleteMethodResponse(response, configMatch);
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
    configMatch.path = parseUri(request.getPath(), configMatch); // transform the uri request to match in the server

    // create a class with the server, location, root, index and all context matching the request
    response = dispatchingResponse(response, request, configMatch);

    return response;
}