#include "responseConstructor.hpp"

bool    isAcceptedMethod(std::vector<std::string> methods, std::string methodRequired)
{
    for (size_t i = 0; i < methods.size(); i++)
        if (methods[i] == methodRequired)
            return true;
    return false;
}

Response    errorResponse(Response &response, t_configMatch  &configMatch, int status)
{
    std::map<int, std::string>::iterator  errorPage = Configuration::getInstance().getErrorPages().find(status);
    
    response.setStatus(status);
    if (errorPage != Configuration::getInstance().getErrorPages().end()
        && !errorPage->second.empty())
    {
        File    errorPath(configMatch.root + errorPage->second);
        if (errorPath.isRegularFile())
        {
            response.setContent(errorPath.getFileContent(), "text/html");
            return response;
        }
    }

    std::vector<unsigned char> defaultErrorPage = html::buildErrorHtmlPage(myItoa(status));
    response.setContent(defaultErrorPage, "text/html");

    return response;
}

Response    autoIndexResponse(Response &response, std::string path)
{
    std::vector<unsigned char> autoIndexPage;

    response.setStatus(OK);
    autoIndexPage = html::buildAutoIndexPage(path);
    response.setContent(autoIndexPage, "text/html");
    
    return response;
}

Response    indexResponse(Response &response,std::string path, std::string index)
{
    File indexFile(path + index);
    Mime indexExtension(getExtension(index));

    response.setStatus(OK);
    response.setContent(indexFile.getFileContent(), indexExtension.getMime());
    
    return response;
}

Response    redirectionResponse(Response &response, std::pair<int, std::string> redirection)
{
    response.setStatus(redirection.first);
    response.setHeader("Location", redirection.second);
    response.setContent(html::buildRedirectionPage(redirection), "text/html");

    return response;
}

void        removeLastBlock(std::string& path)
{
    std::string::reverse_iterator rit = path.rbegin();
    for ( ; rit != path.rend(); rit++)
    {
        if (*rit == '/')
        {
            if (rit == path.rbegin())
            {
                continue ;
            }
            else
            {
                path.erase(rit.base(), path.end());
                return;
            }
            
        }
    }
    path.clear();
}

/*
** transform http://./wordpress/wp-admin/ in htpp://127.0.0.1:8080/wordpress/wp-admin/
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
            // else if (block == "../" || block == "..")
            // {   
            //     removeLastBlock(newPath);
            // }
            else 
                newPath += block;
            block.clear();
        }
        
    }
    return newPath;
}

Response    cgiResponse(std::string cgiResponse, Response &response, t_configMatch  &configMatch)
{
    size_t i = 0; 
    
    while (cgiResponse.find("\r\n", i) != std::string::npos)
    {
        if (cgiResponse.find("Content-type:", i) == i)
        {
            response.setHeader("Content-Type", cgiResponse.substr(i + 14, cgiResponse.find("\r\n", i) - i - 14));
        }
        if (cgiResponse.find("Status:", i) == i)
        {
            int code = strtol(cgiResponse.substr(i + 8, 3).c_str(), NULL, 10);
            code == 0 ? response.setStatus(OK) : response.setStatus(code);
        }
        if (cgiResponse.find("Location:", i) == i)
        {
            response.setHeader("Location", cgiResponse.substr(i + 10, cgiResponse.find("\r\n", i) - i - 10));
        }
        i += cgiResponse.find("\r\n", i) - i + 2;
        if (cgiResponse.find("\r\n", i) == i)
        {
            i += 2;
            break ;
        }
    }
    
    if (response.getHttpStatus().getCode() >= 400)
        return errorResponse(response, configMatch, response.getHttpStatus().getCode());
    if (response.getHttpStatus().getCode() >= 300)
    {
        // std::cerr << "redirection\n";
        // std::cerr << "status code = " << response.getHttpStatus().getCode() << "\n";
        // std::cerr << "Location = " << response.getHeader("Location") << "\n";
        return redirectionResponse(response, std::make_pair<int, std::string>(response.getHttpStatus().getCode(), treatRelativePath(response.getHeader("Location"))));
    }
    std::vector<unsigned char> body(cgiResponse.begin() + i, cgiResponse.end());
    response.setContent(body, response.getHeader("Content-Type"));

    return response;
}

Response    getMethodResponse(Response &response, t_configMatch &configMatch)
{
    File        path(configMatch.pathTranslated);

    if (!isAcceptedMethod(configMatch.location.getAcceptedMethod(), "GET"))
        return errorResponse(response, configMatch, METHOD_NOT_ALLOWED);

    if (path.isRegularFile())
    {
        // std::cerr << "File -> ok regular file\n";
        Mime    extension(getExtension(configMatch.pathTranslated));
        
        response.setStatus(OK);
        if (getExtension(configMatch.pathTranslated) == "php") // display content file if no cgi
            response.setContent(path.getFileContent(), "text/plain");
        else
            response.setContent(path.getFileContent(), extension.getMime());
        return response;
    }
    else if (path.isDirectory() && configMatch.location.getAutoindex())
    {
        // std::cerr << "Directory -> autoindex\n";
        return autoIndexResponse(response, configMatch.pathTranslated);
    }
    // a traiter en amont en recuperant la pathTranslated car cgi a executer si .php
    // else if (path.isDirectory() && !configMatch.index.empty() && path.fileIsInDirectory(configMatch.index)/* && (configMatch.location.getPath() == "/")*/)
    // {
    //     std::cerr << "Directory -> index\n";     
    //     return indexResponse(response, configMatch.pathTranslated, configMatch.index);
    // }
    else
    {
        // std::cerr << "Error not found\n";
        return errorResponse(response, configMatch, NOT_FOUND);
    }
}

void    parseMultipart(std::list<t_multipart> &p, Request &request, std::string boundary)
{
    std::vector<unsigned char>  content(request.getBody().begin(), request.getBody().end());
    size_t                      contentLength = atoi(request.getHeader("Content-Length").c_str());
    size_t				        i = 0;

    while (i + boundary.size() + 6 <= contentLength)
    {
        i += boundary.size() + 2;   // skip boundary
        if (content[i] == '-' && content[i + 1] == '-')
        {
            i += 4;
            break ;
        }
        i += 2;

        t_multipart     part;       // one part on the multipart
        while (1)                  // parse headers
        {
            size_t start = i;
            while (!(content[i] == '\r' && content[i + 1] == '\n'))
                i++;
            if (content[i] == '\r' && content[i + 1] == '\n')
            {   
                std::string headerline(content.begin() + start, content.begin() + i); 
                std::vector<std::string>    headeParts = splitString(headerline, ':');
                part.headers[headeParts[0]] = headeParts[1];
                i += 2;
                if (content[i] == '\r' && content[i + 1] == '\n')
                {
                    i += 2;
                    break ;     // end of headers
                }
            }
        }

        part.content = &content[i];
        part.length = 0;
        while (i + boundary.size() + 4 < contentLength) // parse content
        {
            if (content[i] == '\r' && content[i + 1] == '\n'
            && content[i + 2] == '-' && content[i + 3] == '-'
			&& !::strncmp(boundary.c_str(), reinterpret_cast<const char*>(&content[i + 4]), boundary.size()))
            {
                i += 2;
                break ;
            }
            ++i;
            ++part.length;
        }
        p.push_back(part);
    }
}

std::string t_multipart::getFilename() const
{
    std::map<std::string, std::string>::const_iterator contentDisp = headers.find("Content-Disposition");
    
    if (contentDisp == headers.end())
        return "";
    size_t i = contentDisp->second.find("filename=\"");
    if (i == std::string::npos)
        return "";
    i += 10;
    size_t start = i;
    while (contentDisp->second[i] != '\"')
        i++;
    return std::string(contentDisp->second.begin() + start, contentDisp->second.begin() + i);
}

Response    multipart(Response &response, Request &request, t_configMatch &configMatch, std::string contentTypeHeader)
{
    // check that we have the 2 parts -> "Content-type: multipart/form-data; boundary"
    if (contentTypeHeader.find(";") == std::string::npos)
        return errorResponse(response, configMatch, BAD_REQUEST);
    std::string     contentType = contentTypeHeader.substr(0, contentTypeHeader.find_first_of(";"));
    std::string     boundary = contentTypeHeader.substr(contentTypeHeader.find_first_of(";") + 1, std::string::npos);

    if (boundary.substr(0, 9) != "boundary=")
        return errorResponse(response, configMatch, BAD_REQUEST);
    boundary.erase(0, 9);
    
    // check if the content is ok and get content original file (without headers and boundary)
    std::list<t_multipart> parts;
    parseMultipart(parts, request, boundary);

    // create and write content into the new file for each part
    std::list<t_multipart>::iterator it = parts.begin();
	while (it != parts.end())
    {
        std::string filename = it->getFilename();
        if (filename.empty()) 
            return errorResponse(response, configMatch, BAD_REQUEST);
        appendToFile(configMatch.root + configMatch.server.getUploadPath() + "/" + filename, reinterpret_cast<char*>(it->content), it->length);
        ++it;
    }

    response.setStatus(CREATED);
	response.setContent(html::buildPage("File upload in : " + configMatch.root + configMatch.server.getUploadPath()), "text/html");

    return response;
}

Response    postMethodResponse(Response &response, Request &request, t_configMatch &configMatch)
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

    // check if it's a multipart/form-data
    if (request.getHeader("Content-Type").find("multipart/form-data") != std::string::npos)
        return multipart(response, request, configMatch, request.getHeader("Content-Type"));

    // create the file
    if (!fileToPost.createFile(pathToUpload, request.getBody()))
        return errorResponse(response, configMatch, INTERNAL_SERVER_ERROR);
    response.setStatus(CREATED);

    // indicate the url of the resource we created thanks to "location" header
    response.setHeader("Location", request.getUri().getUrl()); // need to send the full uri : http://127.0.0.1:8080/file.php   
    response.setContent(html::buildRedirectionPage(std::pair<int, std::string>(201, pathToUpload)), "text/html");
   
    return response;
}

Response    deleteMethodResponse(Response &response, t_configMatch &configMatch)
{
    if (!isAcceptedMethod(configMatch.location.getAcceptedMethod(), "DELETE"))
        return errorResponse(response, configMatch, METHOD_NOT_ALLOWED);
    
    File    fileToDelete(configMatch.pathTranslated);

    if (fileToDelete.exists())
    {
       if (remove(configMatch.pathTranslated.c_str()) == 0)
        {
            response.setStatus(OK);
            response.setContent(html::buildPage("File " + configMatch.pathTranslated + " successfully deleted from server"), "text/html");
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
Response    constructResponse(Request &request, std::string serverName)
{
    // first need to get the server and location to use for this response (context)
    t_configMatch   configMatch = getConfigMatch(request, serverName);
    Response        response;
    File            path(configMatch.pathTranslated);

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