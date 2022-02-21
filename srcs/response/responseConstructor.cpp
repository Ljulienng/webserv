#include "responseConstructor.hpp"

bool    isAcceptedMethod(std::vector<std::string> methods, std::string methodRequired)
{
    for (size_t i = 0; i < methods.size(); i++)
        if (methods[i] == methodRequired)
            return true;
    return false;
}

Response*    errorResponse(Response* response, t_configMatch  &configMatch, int status)
{
    std::map<int, std::string>::iterator  errorPage = Configuration::getInstance().getErrorPages().find(status);
    
    response->setStatus(status);
    if (errorPage != Configuration::getInstance().getErrorPages().end()
        && !errorPage->second.empty())
    {
        File    errorPath(configMatch.root + errorPage->second);
        if (errorPath.isRegularFile())
        {
            response->setContent(errorPath.getFileContent(), "text/html");
            return response;
        }
    }

    std::vector<unsigned char> defaultErrorPage = html::buildErrorHtmlPage(myItoa(status));
    response->setContent(defaultErrorPage, "text/html");

    return response;
}

Response*    autoIndexResponse(Response* response, std::string path)
{
    std::vector<unsigned char> autoIndexPage;

    response->setStatus(OK);
    autoIndexPage = html::buildAutoIndexPage(path);
    response->setContent(autoIndexPage, "text/html");
    
    return response;
}

// plus besoin : on l'a dans la path_translated donc on passe dans une getResponse
// Response*    indexResponse(Response* response,std::string path, std::string index)
// {
//     File indexFile(path + index);
//     Mime indexExtension(getExtension(index));

//     response->setStatus(OK);
//     response->setContent(indexFile.getFileContent(), indexExtension.getMime());
    
//     return response;
// }

Response*    redirectionResponse(Response* response, std::pair<int, std::string> redirection)
{
    response->setStatus(redirection.first);
    response->setHeader("Location", redirection.second);
    response->setContent(html::buildRedirectionPage(redirection), "text/html");

    return response;
}

/*
** transform http://./wordpress/wp-admin/ in htpp://127.0.0.1:8080/wordpress/wp-admin/
** A CREUSER -> pb avec redirections wordpress
*/
std::string     treatRelativePath(std::string path)
{
    std::string newPath;
    std::string block;
    
    if (path.substr(0, 7) == "http://")
        if (path[7] == '.')
            newPath += "http://127.0.0.1:8080/";
    for (size_t i = 7; i < path.size(); i++)
    {
        block += path[i];
        if (path[i] == '/' || i == (path.size() - 1))
        {
            if (block == "./" || block == ".")
            {
                block.clear();
                continue;
            }
            else 
                newPath += block;
            block.clear();
        }
    }
    return newPath;
}

Response*    cgiResponse(Response* response, std::string cgiResponse, t_configMatch  &configMatch)
{
    size_t i = 0; 

    while (cgiResponse.find("\r\n", i) != std::string::npos)
    {
        if (cgiResponse.find("Content-type:", i) == i)
        {
            response->setHeader("Content-Type", cgiResponse.substr(i + 14, cgiResponse.find("\r\n", i) - i - 14));
        }
        if (cgiResponse.find("Status:", i) == i)
        {
            int code = strtol(cgiResponse.substr(i + 8, 3).c_str(), NULL, 10);
            code == 0 ? response->setStatus(OK) : response->setStatus(code);
        }
        if (cgiResponse.find("Location:", i) == i)
        {
            response->setHeader("Location", cgiResponse.substr(i + 10, cgiResponse.find("\r\n", i) - i - 10));
        }
        i += cgiResponse.find("\r\n", i) - i + 2;
        if (cgiResponse.find("\r\n", i) == i)
        {
            i += 2;
            break ;
        }
    }
    
    if (response->getHttpStatus().getCode() >= 400)
        return errorResponse(response, configMatch, response->getHttpStatus().getCode());
    if (response->getHttpStatus().getCode() >= 300)
    {
        // std::cerr << "status code = " << response.getHttpStatus().getCode() << "\n";
        // std::cerr << "Location = " << response.getHeader("Location") << "\n";
        return redirectionResponse(response, std::make_pair<int, std::string>(response->getHttpStatus().getCode(), treatRelativePath(response->getHeader("Location"))));
    }
    std::vector<unsigned char> body(cgiResponse.begin() + i, cgiResponse.end());
    response->setContent(body, response->getHeader("Content-Type"));

    return response;
}

Response*    getMethodResponse(Response *response, t_configMatch &configMatch)
{
    File        path(configMatch.pathTranslated);

    if (!isAcceptedMethod(configMatch.location.getAcceptedMethod(), "GET"))
        return errorResponse(response, configMatch, METHOD_NOT_ALLOWED);

    if (path.isRegularFile())
    {
        Mime    extension(getExtension(configMatch.pathTranslated));
        
        response->setStatus(OK);
        // new version : just create file before to pass in poll() to read the fd 
        if (response->setPollFdFileToRead(path.getFilePath().c_str()) == false)
            return errorResponse(response, configMatch, INTERNAL_SERVER_ERROR);
        response->addFile();
        response->setContentType(extension.getMime());
        
        /* ancienne version qui marche mais sans repasser par poll()
        if (getExtension(configMatch.pathTranslated) == "php") // display content file if no cgi
            response.setContent(path.getFileContent(), "text/plain");
        else
            response.setContent(path.getFileContent(), extension.getMime());
        */
        return response;
    }
    else if (path.isDirectory() && configMatch.location.getAutoindex())
        return autoIndexResponse(response, configMatch.pathTranslated);
    // a traiter en amont en recuperant la pathTranslated car cgi a executer si .php
    // else if (path.isDirectory() && !configMatch.index.empty() && path.fileIsInDirectory(configMatch.index)/* && (configMatch.location.getPath() == "/")*/)
    // {
    //     std::cerr << "Directory -> index\n";     
    //     return indexResponse(response, configMatch.pathTranslated, configMatch.index);
    // }
    else
        return errorResponse(response, configMatch, NOT_FOUND);
}

Response*    postMethodResponse(Response* response, Request &request, t_configMatch &configMatch)
{ 
    File            fileToPost(configMatch.pathTranslated);
    size_t          lastSlash = configMatch.pathTranslated.find_last_of('/');
    std::string     filename = std::string(configMatch.pathTranslated.begin() + lastSlash, configMatch.pathTranslated.end());
    std::string     pathToUpload = configMatch.root + configMatch.server.getUploadPath() + filename;

    if (!isAcceptedMethod(configMatch.location.getAcceptedMethod(), "POST"))
        return errorResponse(response, configMatch, METHOD_NOT_ALLOWED); // method not allowed

    // check we have a directory to uploads files else errorResponse
    if (configMatch.server.getUploadPath() == "")
        return errorResponse(response, configMatch, FORBIDDEN);

    // upload file
    if (request.getHeader("Content-Type").find("multipart/form-data") != std::string::npos)
        return multipart(response, request, configMatch, request.getHeader("Content-Type"));

    // new version : just create file before to pass in poll() to write the fd 
    // upload message/text
    if (response->setPollFdFileToWrite(pathToUpload.c_str(), false, request.getBody()) == false)
        return errorResponse(response, configMatch, INTERNAL_SERVER_ERROR);
    response->addFile();

    // ancienne version qui marche mais sans repasser par poll()
    // create the file
    // if (!fileToPost.createFile(pathToUpload, request.getBody()))
    //     return errorResponse(response, configMatch, INTERNAL_SERVER_ERROR);

    response->setStatus(CREATED);

    // indicate the url of the resource we created thanks to "location" header
    response->setHeader("Location", request.getUri().getUrl()); // need to send the full uri : http://127.0.0.1:8080/file.php   
    response->setContent(html::buildRedirectionPage(std::pair<int, std::string>(201, pathToUpload)), "text/html");
   
    return response;
}

Response*    deleteMethodResponse(Response* response, t_configMatch &configMatch)
{
    if (!isAcceptedMethod(configMatch.location.getAcceptedMethod(), "DELETE"))
        return errorResponse(response, configMatch, METHOD_NOT_ALLOWED);
    
    File    fileToDelete(configMatch.pathTranslated);

    if (fileToDelete.exists())
    {
       if (remove(configMatch.pathTranslated.c_str()) == 0)
        {
            response->setStatus(OK);
            response->setContent(html::buildPage("File " + configMatch.pathTranslated + " successfully deleted from server"), "text/html");
            return response;
        }
        return errorResponse(response, configMatch, NO_CONTENT);
    }
    else
        return errorResponse(response, configMatch, NO_CONTENT);
}

/*
** evaluate the type of the response we need :
**      - get
**      - post
**      - delete
**      - redirection
**      - error
*/
Response*    constructResponse(Response* response, Request &request, t_configMatch configMatch)
{
    File    path(configMatch.pathTranslated);

    if (path.isDirectory() && configMatch.pathTranslated[configMatch.pathTranslated.size() - 1] != '/')
        return redirectionResponse(response, std::pair<int, std::string>(MOVED_PERMANENTLY, request.getPath() + "/"));
    else if (configMatch.location.getRedirection().first > 0 && !configMatch.location.getRedirection().second.empty())
        return redirectionResponse(response, configMatch.location.getRedirection());
    else if (request.getMethod() == "GET")
        return getMethodResponse(response, configMatch);
    else if (request.getMethod() == "POST")
        return postMethodResponse(response, request, configMatch);
    else if (request.getMethod() == "DELETE")
        return deleteMethodResponse(response, configMatch);
    return errorResponse(response, configMatch, NOT_FOUND);
}