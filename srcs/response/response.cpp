#include "response.hpp"

void            Response::_updateMessage()
{
    // append version
    _message = _httpVersion + " ";

    // append http status
    std::stringstream   stream;
    stream << _httpStatus.getCode();
    _message += stream.str() + " " + _httpStatus.getMessage() + "\r\n";;

    // append headers
    std::map<std::string, std::string>::iterator headerIterator = _headers.begin();
    for ( ; headerIterator != _headers.end(); headerIterator++)
        _message += headerIterator->first + ": " + headerIterator->second + "\r\n";
    _message += "\r\n";
    
    // append content
    _message += _content;
}

/* SETTERS */
void        Response::setHeader(std::string key, std::string value)
{
    _headers.insert(std::pair<std::string, std::string>(key, value));
}

void        Response::setContent(std::string content, std::string contentType)
{
    _content = content;
    setHeader("Content-Type", contentType);
    setHeader("Content-Length", utils::myItoa(content.size()));
}

/* GETTERS */
/* get all headers */
std::map<std::string, std::string>         &Response::getHeaders()
{ return  _headers; }

/* get one header value thanks to its key */
std::string         Response::getHeader(std::string key)
{
    if (getHeaders().find(key) != getHeaders().end())
        return (getHeaders().find(key)->second);
    return std::string("");
}

std::string         &Response::getHttpVersion()
{ return  _httpVersion; }

HttpStatus          &Response::getHttpStatus()
{ return _httpStatus; }

std::string         &Response::getContent()
{ return  _content; }

std::string         &Response::getMessage()
{
    _updateMessage();
    return  _message;
}


// std::string     parseUrl(Server &server, Location &location, std::string url, std::string index, std::string root)
// {
//     std::string newUrl;
//     (void)index;
//     (void)server;
//     (void)location;

//     // if uri is file ->
//     //          - if exist : display content
//     //          - else : 404 error
//     // if uri is folder ->
//     //          - if doesn't exist : 404 error
//     //          - else :
//     //                    - autoindex on : display list of file
//     //                    - autoindex off : 
//     //                                      - if default file : display
//     //                                      - else : 404 error

//     // test
//     // File    indexFile(root + url + index);
    
//     // if (url[url.size() - 1] == '/' && indexFile.isRegularFile())    // if directory and directory/index exist
//     //     newUrl = root + url + index;
//     // else                                                         // file or directory without autoindex
//     //     newUrl = root + url;
//     newUrl = root + url;
//     return newUrl;
// }

// std::string     getExtension(std::string filename)
// {
//     size_t index = filename.find_last_of(".") + 1;
//     if (index == std::string::npos)
//         return NULL;
//     return (filename.substr(index, std::string::npos));
// }


// void    Response::_buildErrorResponse(std::string root, int status)
// {
//     std::string     errorPage = Configuration::getInstance().getErrorPages()[status];
//     File            errorPath(root + errorPage);

//     _httpStatus.setStatus(status);
//     if (!errorPage.empty() && errorPath.isRegularFile())
//     {
//         setContent(errorPath.getFileContent(), "text/html");
//     }
//     else
//     {
//         std::string defaultErrorPage = html::buildErrorHtmlPage(utils::myItoa(status));
//         setContent(defaultErrorPage, "text/html");
//     }
// }

// void    Response::_buildAutoIndexResponse(std::string path)
// {
//     std::string autoIndexPage;

//     _httpStatus.setStatus(200);
//     autoIndexPage = html::buildAutoIndexPage(path);
//     setContent(autoIndexPage, "text/html");
    
// }

// void    Response::_buildIndexResponse(std::string path, std::string index)
// {
//         File indexFile(path + index);
//         Mime indexExtension(getExtension(index));

//         _httpStatus.setStatus(200);
//         setContent(indexFile.getFileContent(), indexExtension.getMime()); // set content-type + content-length + content
// }

// void       Response::_redirectionResponse(std::pair<int, std::string> redirection)
// {
//     std::string redirectionPage;

//     std::cout << "Redirection " << redirection.first << " -> " << redirection.second << "\n";
//     _httpStatus.setStatus(redirection.first);
//     setHeader("Location", redirection.second);
//     redirectionPage = html::buildRedirectionPage(redirection);
//     setContent(redirectionPage, "text/html");
// }

// void    Response::_cgiResponse()
// {
//     std::cout << "Need to execute CGI\n";
// }

// void    Response::_getMethodResponse(Location &location, std::string _path, std::string index, std::string root)
// {
//     File        path(_path);
    
//     if (path.isRegularFile())
//     {
//         std::cout << "File -> ok regular file\n";
//         Mime    extension(getExtension(_path));

//         _httpStatus.setStatus(200);
//         setContent(path.getFileContent(), extension.getMime()); // set content-type + content-length + content
//     }
//     else if (path.isDirectory() && location.getAutoindex())
//     {
//         std::cout << "Directory -> autoindex\n";
//         _buildAutoIndexResponse(_path);
//     }
//     else if (path.isDirectory() && !location.getAutoindex() && !index.empty() && (location.getPath() == "/"))
//     {
//         std::cout << "Directory -> index\n";
//         _buildIndexResponse(_path, index);

//     }
//     else // not found
//     {
//         std::cout << "Error not found\n";
//         _buildErrorResponse(root, 404); // send error response and page 404.html
//     }
// }

// void    Response::_postMethodResponse()
// {

// }

// void    Response::_deleteMethodResponse(std::string path, std::string root)
// {
//     File    fileToDelete(path);

//     if (fileToDelete.isRegularFile())
//     {
//        if (remove(path.c_str()) == 0)
//         {
//             _httpStatus.setStatus(200);
//             setContent(html::buildPage("Method DELETE ok : file successfully deleted"), "text/html");
//         }
//         else
//             _buildErrorResponse(root, 204);
//     }
//     else
//         _buildErrorResponse(root, 204);
// }

// /*
// ** evaluate the type of the response we need :
// **      - get
// **      - post
// **      - delete
// **      - cgi
// **      - redirection
// **      - error
// */
// void      Response::_dispatchingResponse(Request &request, Server &server, Location &location, std::string index, std::string root)
// {
//     // then we need to transform the uri request to match in the server
//     std::string path = parseUrl(server, location, request.getPath(), index, root); //TO IMPLEMENT

//     if (request.getPath().find(Configuration::getInstance().getCgi().first) != std::string::npos
//         && (request.getMethod() == "GET" || request.getMethod() == "POST"))
//         _cgiResponse();
//     else if (location.getRedirection().first > 0 && !location.getRedirection().second.empty())
//         _redirectionResponse(location.getRedirection());
//     else if (request.getMethod() == "GET")
//         _getMethodResponse(location, path, index, root);
//     else if (request.getMethod() == "POST")
//         _postMethodResponse();
//     else if (request.getMethod() == "DELETE")
//         _deleteMethodResponse(path, root);
// }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Response::Response() : 
        _headers(),
        _httpVersion("HTTP/1.1"),
        _httpStatus(),
        _content(),
        _message()
{
    // we set some headers
    setHeader("Server", "Webserv_42");
    setHeader("Date", utils::getTimestamp());
    setHeader("Connection", "keep-alive");
}


// Response::Response(Request &request, std::string serverName) : 
//         _headers(),
//         _httpVersion("HTTP/1.1"),
//         _httpStatus(200),
//         _content(),
//         _message()
// {
//     // we set some headers
//     setHeader("Server", "Webserv_42");
//     request.getHeader("Connection") == "close" ? setHeader("Connection", "close") : setHeader("Connection", "keep-alive");
//     setHeader("Date", utils::getTimestamp());

//     // first need to get the server and location to use for this response (context)
//     Server &server = Configuration::getInstance().findServer(serverName);
//     Location &location = server.findLocation(request.getPath());
//     std::string root;
//     std::string index;

//     location.getRoot().empty() ? root = server.getRoot() : root = location.getRoot();
//     location.getIndex().empty() ? index = server.getIndex() : index = location.getIndex();
    
//     // create a class with the server, location, root, index and all context matching the request
//     _dispatchingResponse(request, server, location, index, root);
// }

Response::Response(const Response &src)
{
	*this = src;
}

Response::~Response(){}

Response &Response::operator=(const Response &src)
{
	if (&src != this)
	{
		_headers = src._headers;
        _httpVersion = src._httpVersion;
        _httpStatus = src._httpStatus;
        _content = src._content;
        _message = src._message;
		// to be completed if new attributes
	}
	return (*this);
}