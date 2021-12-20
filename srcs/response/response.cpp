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

std::string     parseUri(Server &server, Location &location, std::string uri)
{
    (void)server;
    std::string newUri;
    std::string root;
    std::string defaultFile;

    if (location.getRoot().empty())
        root = "./www/data" ; // server.getRoot(); // a inclure dans Server
    else
        root = location.getRoot();

    if (location.getDefaultFile().empty())
        defaultFile = "index.html"; // server.getDefaultFile(); // a inclure dans Server
    else
        defaultFile = location.getDefaultFile();

    // test
    if (uri[uri.size() - 1] == '/')
        newUri = root + uri + defaultFile;
    else 
        newUri = root + uri + "/" + defaultFile;

    return newUri;
}

std::string     getExtension(std::string filename)
{
    size_t index = filename.find_last_of(".") + 1;
    if (index == std::string::npos)
        return NULL;
    return (filename.substr(index, std::string::npos));
}

void    Response::_makeErrorResponse(int status)
{
    File errorFile("./www/data/error_pages/404.html");
    _httpStatus.setStatus(status);
    setContent(errorFile.getFileContent(), "text/html");
}

void    Response::_getResponse(std::string _path)
{
    File        path(_path);
    Mime        extension(getExtension(_path));
    // std::cout << "extension = " << getExtension(absolutePath) << "\n";
    // std::cout << "mime = " << extension.getMime() << "\n";
    
    if (path.isRegularFile())
    {
        std::cout << "Regular file\n";
        std::string mime = extension.getMime();
        // set status ok
        _httpStatus.setStatus(200);
        // set content-type + content-length + content
        setContent(path.getFileContent(), extension.getMime());
    }
    else
    {
        std::cout << "400 error\n";
        // send error response and page 404.html
        _makeErrorResponse(400);
    }
}

void    _postResponse()
{

}

void    _deleteResponse()
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
void      Response::_dispatchingResponse(Request &request, Server &server, Location &location)
{
    // then we need to transform the uri request to match in the server
    std::string path = parseUri(server, location, request.getPath()); //TO IMPLEMENT
    std::cout << "Path = " << path << "\n";
    if (request.getMethod() == "GET")
        _getResponse(path);
    else if (request.getMethod() == "POST")
        _postResponse();
    else if (request.getMethod() == "DELETE")
        _deleteResponse();
}

Response::Response(Request &request, Configuration &config, std::string serverName) : 
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
    Server &server = config.findServer(serverName);
    Location &location = server.findLocation(request.getPath());
    _dispatchingResponse(request, server, location);
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