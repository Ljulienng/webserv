#include "responseConstructor.hpp"

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

bool    isAcceptedMethod(std::vector<std::string> methods, std::string methodRequired)
{
    for (size_t i = 0; i < methods.size(); i++)
        if (methods[i] == methodRequired)
            return true;
    return false;
}

Response    multipart(Response &response, Request &request, t_configMatch &configMatch, std::string contentTypeHeader)
{
    (void)response;(void)request;(void)configMatch;
    std::cout << "[multipart]\n";

    // check that we have the 2 parts -> "Content-type: multipart/form-data; boundary"
    if (contentTypeHeader.find(";") == std::string::npos)
        return errorResponse(response, configMatch, 400);
    std::string     contentType = contentTypeHeader.substr(0, contentTypeHeader.find_first_of(";"));
    std::string     boundary = contentTypeHeader.substr(contentTypeHeader.find_first_of(";") + 1, std::string::npos);

    if (boundary.substr(0, 9) != "boundary=")
        return errorResponse(response, configMatch, 400);
    boundary.erase(0, 9);

    // check if the content is ok and get content original file (without headers and boundary)

    // get the filename to upload thanks to the headers "filename="

    // create and write content into the new file

    // set status and content

    return response;
}

Response    postMethodResponse(Response &response, Request &request, t_configMatch &configMatch)
{
    File            fileToPost(configMatch.path);
    size_t          lastSlash = configMatch.path.find_last_of('/');
    std::string     filename = std::string(configMatch.path.begin() + lastSlash, configMatch.path.end());
    std::string     pathToUpload = configMatch.root + configMatch.server.getUploadPath() + filename;

    // CHECK IF IT'S AN ALLOWED METHOD
    if (!isAcceptedMethod(configMatch.location.getAcceptedMethod(), "POST"))
        return errorResponse(response, configMatch, 405); // method not allowed

    // check we have a directory to uploads files else errorResponse
    if (configMatch.server.getUploadPath() == "")
        return errorResponse(response, configMatch, 403); // forbidden

    // check if it's a multipart/form-data
    if (request.getHeader("Content-Type").find("multipart/form-data") != std::string::npos)
        return multipart(response, request, configMatch, request.getHeader("Content-Type"));

    // we create the file
    // std::cout << "[postMethodResponse] configMatch.path = " << configMatch.path << "\n";
    // std::cout << "filename = " << filename << "\n";
    // std::cout << "[postMethodResponse] body = " << request.getBody() << "\n";
    if (!fileToPost.createFile(pathToUpload, request.getBody()))
        return errorResponse(response, configMatch, 500);
    response.setStatus(201);

    // we indicate the url of the resource we created thanks to "location" header
    // response.setHeader("Location", request.getPath()); // need to send the full uri : http://127.0.0.1:8080/file.ext
    std::string fullUri = "http://" + configMatch.server.getIp() + ":" + utils::myItoa(configMatch.server.getPort()) + request.getPath();
    // std::cout << "[postMethodResponse] fullUri = " << fullUri << "\n";
    response.setHeader("Location", fullUri); //provisoire en attendant de l'avoir via la requete
    
    response.setContent(html::buildRedirectionPage(std::pair<int, std::string>(201, pathToUpload)), "text/html");
   
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
    else if (configMatch.server.getCgi().first == ".php"
        && request.getPath().find(configMatch.server.getCgi().first) != std::string::npos
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
    configMatch.path = getServerPath(request.getPath(), configMatch); // transform the uri request to match in the server

    // create a class with the server, location, root, index and all context matching the request
    response = dispatchingResponse(response, request, configMatch);

    return response;
}