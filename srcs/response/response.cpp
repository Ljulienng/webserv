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

    /**************** TEST *************/   
    // std::string content = "<html><body><h1>Hello world !</h1></body></html>";   
    // _message += "Content-length: " + utils::myItoa(content.size()) + "\r\n";
    // _message += "Content-Type: text/html; charset=UTF-8\r\n";
    // _message += "\r\n";
    // _message += content;
    /***********************************/
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

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Response::Response() :
        _headers(),
        _httpVersion(),
        _httpStatus(200),
        _content(),
        _message()
			// to be completed if new attributes
{}



std::string     parseUrl(Server &server, Location &location, std::string uri)
{
    (void)server;
    std::string newUri;
    std::string root;
    std::string index;

    if (location.getRoot().empty())
        root = server.getRoot() ; // server.getRoot(); // a inclure dans Server
    else
        root = location.getRoot();

    if (location.getIndex().empty())
        index = server.getIndex(); // server.getDefaultFile(); // a inclure dans Server
    else
        index = location.getIndex();

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
    File indexFile(root + uri + index);
    if (uri[uri.size() - 1] == '/' && indexFile.isRegularFile()) // if directory and directory/index exist
        newUri = root + uri + index;
    else    // file or directory without autoindex
        newUri = root + uri;
    return newUri;
}

std::string     getExtension(std::string filename)
{
    size_t index = filename.find_last_of(".") + 1;
    if (index == std::string::npos)
        return NULL;
    return (filename.substr(index, std::string::npos));
}


void    Response::_buildErrorResponse(std::string root, int status)
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

void    Response::_buildAutoIndexResponse(std::string path)
{
    std::string autoIndexPage;

    _httpStatus.setStatus(200);
    autoIndexPage = html::buildAutoIndexPage(path);
    setContent(autoIndexPage, "text/html");
    
}

void       Response::_redirectionResponse(std::pair<int, std::string> redirection)
{
    std::string redirectionPage;

    _httpStatus.setStatus(redirection.first);
    setHeader("Location", redirection.second);
    redirectionPage = html::buildRedirectionPage(redirection);
    setContent(redirectionPage, "text/html");
}

void    Response::_getMethodResponse(Location &location, std::string _path, std::string index, std::string root)
{
    (void)index;
    File        path(_path);
    Mime        extension(getExtension(_path));

    if (path.isRegularFile())
    {
        std::cout << "Regular file\n";
        _httpStatus.setStatus(200); // ok
        setContent(path.getFileContent(), extension.getMime()); // set content-type + content-length + content
    }
    else if (path.isDirectory() && location.getAutoindex())
    {
        std::cout << "Autoindex\n";
        _buildAutoIndexResponse(_path);
    }
    else // not found
    {
        std::cout << "Error not found\n";
        _buildErrorResponse(root, 404); // send error response and page 404.html
    }
}

void    _postMethodResponse()
{

}

void    _deleteMethodResponse()
{

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
void      Response::_dispatchingResponse(Request &request, Server &server, Location &location, std::string index, std::string root)
{
    // then we need to transform the uri request to match in the server
    std::string path = parseUrl(server, location, request.getPath()); //TO IMPLEMENT

    if (location.getRedirection().first > 0 && !location.getRedirection().second.empty())
        _redirectionResponse(location.getRedirection());
    else if (request.getMethod() == "GET")
        _getMethodResponse(location, path, index, root);
    else if (request.getMethod() == "POST")
        _postMethodResponse();
    else if (request.getMethod() == "DELETE")
        _deleteMethodResponse();
}

Response::Response(Request &request, std::string serverName) : 
        _headers(),
        _httpVersion("HTTP/1.1"),
        _httpStatus(200),
        _content(),
        _message()
{
    // we set some headers
    setHeader("Server", "Webserv_42");
    if (request.getHeader("Connection") == "close")
        setHeader("Connection", "close");
    else
        setHeader("Connection", "keep-alive");
    setHeader("Date", utils::getTimestamp());


    // first need to get the server and location to use for this response (context)
    Server &server = Configuration::getInstance().findServer(serverName);
    Location &location = server.findLocation(request.getPath());
    std::string root;
    std::string index;

    if (location.getRoot().empty())
        root = server.getRoot() ; // server.getRoot(); // a inclure dans Server
    else
        root = location.getRoot();

    if (location.getIndex().empty())
        index = server.getIndex(); // server.getDefaultFile(); // a inclure dans Server
    else
        index = location.getIndex();
    
    // create a class with the server, location, root, index and all context matching the request
    _dispatchingResponse(request, server, location, index, root);
}

Response::Response(const Response &src)
{
	*this = src;
}

Response::~Response() {}

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