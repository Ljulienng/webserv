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

    std::vector<unsigned char> defaultErrorPage = html::buildErrorHtmlPage(utils::myItoa(status));
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
    response.setContent(indexFile.getFileContent(), indexExtension.getMime()); // set content-type + content-length + content
    
    return response;
}

/*
** 1) set env variables [Julien]
** 2) execute cgi [Julien]
** 3) get cgi response in stdin and put it in a vector<unsigned char> [Julien]
** 4) parse the cgi response [Elie]
** 5) build the server response [Elie]
*/
Response    cgiResponse(Response &response, Request &request, t_configMatch  &configMatch)
{
    std::cout << "Execute CGI\n";

    // 4)
    cgiConstructor  cgi;
    cgi.initHeaders(request, configMatch);
    std::vector<unsigned char> ret = cgi.execCgi();
    std::string cgiResponse = std::string(ret.begin(), ret.end()); // provisoire

    /***** TEST *********/
    //    std::string cgiResponse = "Blabla:\r\nContent-type:html\r\nStatus:200ok\r\n\r\nbody is here"; // a remplacer par le retour de Julien
    /*************************/
    size_t i = 0;
    
    while (cgiResponse.find("\r\n", i) != std::string::npos)
    {
        if (cgiResponse.find("Content-type:", i) == i)
                response.setHeader("Content-type", cgiResponse.substr(i + 13, cgiResponse.find("\r\n", i) - i - 13));
        if (cgiResponse.find("Status:", i) == i)
        {
            int code = strtol(cgiResponse.substr(i + 7, 3).c_str(), NULL, 10);
            code == 0 ? response.setStatus(OK) : response.setStatus(code);
        }
        i += cgiResponse.find("\r\n", i) - i + 2;
        if (cgiResponse.find("\r\n", i) == i)
        {
            i += 2;
            break ;
        }
    }
    std::vector<unsigned char> body(cgiResponse.begin() + i, cgiResponse.end());

    // 5)
    if (response.getHttpStatus().getCode() >= 400)
        return errorResponse(response, configMatch, response.getHttpStatus().getCode());
    response.setContent(body, response.getHeader("Content-type"));

    return response;
}

Response    redirectionResponse(Response &response, std::pair<int, std::string> redirection)
{
    // std::cout << "Redirection " << redirection.first << " -> " << redirection.second << "\n";
    response.setStatus(redirection.first);
    response.setHeader("Location", redirection.second);
    response.setContent(html::buildRedirectionPage(redirection), "text/html");

    return response;
}

Response    getMethodResponse(Response &response, t_configMatch &configMatch)
{
    File        path(configMatch.path);

    if (!isAcceptedMethod(configMatch.location.getAcceptedMethod(), "GET"))
        return errorResponse(response, configMatch, METHOD_NOT_ALLOWED); // method not allowed

    if (path.isRegularFile())
    {
        std::cout << "File -> ok regular file\n";
        Mime    extension(getExtension(configMatch.path));

        response.setStatus(OK);
        response.setContent(path.getFileContent(), extension.getMime());
        return response;
    }
    else if (path.isDirectory() && configMatch.location.getAutoindex())
    {
        std::cout << "Directory -> autoindex\n";
        return autoIndexResponse(response, configMatch.path);
    }
    else if (path.isDirectory() && !configMatch.index.empty() && path.fileIsInDirectory(configMatch.index)/* && (configMatch.location.getPath() == "/")*/)
    {
        std::cout << "Directory -> index\n";     
        return indexResponse(response, configMatch.path, configMatch.index);
    }
    else
    {
        std::cout << "Error not found\n";
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
    std::cout << "[multipart]\n";

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

    response.setStatus(201);
	response.setContent(html::buildPage("File upload"), "text/html");

    return response;
}

Response    postMethodResponse(Response &response, Request &request, t_configMatch &configMatch)
{
    File            fileToPost(configMatch.path);
    size_t          lastSlash = configMatch.path.find_last_of('/');
    std::string     filename = std::string(configMatch.path.begin() + lastSlash, configMatch.path.end());
    std::string     pathToUpload = configMatch.root + configMatch.server.getUploadPath() + filename;

    if (!isAcceptedMethod(configMatch.location.getAcceptedMethod(), "POST"))
        return errorResponse(response, configMatch, METHOD_NOT_ALLOWED); // method not allowed

    // check we have a directory to uploads files else errorResponse
    if (configMatch.server.getUploadPath() == "")
        return errorResponse(response, configMatch, FORBIDDEN);

    // check if it's a multipart/form-data
    if (request.getHeader("Content-Type").find("multipart/form-data") != std::string::npos)
        return multipart(response, request, configMatch, request.getHeader("Content-Type"));

    // we create the file
    if (!fileToPost.createFile(pathToUpload, request.getBody()))
        return errorResponse(response, configMatch, INTERNAL_SERVER_ERROR);
    response.setStatus(CREATED);

    // we indicate the url of the resource we created thanks to "location" header
    // response.setHeader("Location", request.getPath()); // need to send the full uri : http://127.0.0.1:8080/file.ext
    std::string fullUri = "http://" + configMatch.server.getIp() + ":" + utils::myItoa(configMatch.server.getPort()) + request.getPath();
    // std::cout << "[postMethodResponse] fullUri = " << fullUri << "\n";
    response.setHeader("Location", fullUri); // PROVISOIRE EN ATTENDANT DE L'AVOIR VIA LA REQUETE
    
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
            response.setStatus(OK);
            response.setContent(html::buildPage("Method DELETE ok : file successfully deleted"), "text/html");
            return response;
        }
        else
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
**      - cgi
**      - redirection
**      - error
*/
Response    dispatchingResponse(Response &response, Request &request, t_configMatch  &configMatch)
{
    File        path(configMatch.path);

    if (path.isDirectory() && configMatch.path[configMatch.path.size() - 1] != '/')
        return redirectionResponse(response, std::pair<int, std::string>(MOVED_PERMANENTLY, request.getPath() + "/"));
    else if (configMatch.server.getCgi().first == ".php"
        && request.getPath().find(configMatch.server.getCgi().first) != std::string::npos
        && (request.getMethod() == "GET" || request.getMethod() == "POST"))
        return cgiResponse(response, request, configMatch);
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