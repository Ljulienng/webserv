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
    // _message += "\r\n";
    
    // // append content
    // _message += _content;

    /**************** TEST *************/   
    std::string content = "<html><body><h1>Hello world !</h1></body></html>";   
    _message += "Content-length: " + utils::myItoa(content.size()) + "\r\n";
    _message += "Content-Type: text/html; charset=UTF-8\r\n";
    _message += "\r\n";
    _message += content;
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

void        Response::setStatus(HttpStatus status)
{
    _httpStatus = status;
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

// std::string     parseUri(std::string uri)
// {
//     std::string newUri;
//     std::string delimiter = "/";
// 	std::list<std::string> mylist;

//     size_t pos = 0;
// 	std::string token;
//     while ((pos = uri.find(delimiter)) != std::string::npos)
//     {
// 		token = uri.substr(0, pos);
// 		if (token == "..")
// 		{
// 			if (!mylist.empty())
// 				mylist.pop_back();
// 		}
// 		else if (token == "."|| token == "")
// 			;
// 		else 
// 		{
// 			mylist.push_back("/" + token);
// 		}
// 		uri.erase(0, pos + delimiter.length());
// 	}

//     if (token == "..")
// 	{
// 		if(!mylist.empty())
// 			mylist.pop_back();
// 	}
// 	else if (uri == ".")
// 		;
// 	else 
// 		mylist.push_back("/" + uri);

//     if (mylist.empty())
// 		newUri = "/";
// 	for (std::list<std::string>::iterator it=mylist.begin(); it != mylist.end(); ++it)
// 		newUri += *it;

//     return newUri;
// }

void    Response::_getResponse()
{

}

void    Response::_postResponse()
{

}

void    Response::_deleteResponse()
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
    (void)server; (void)location;
    // then we need to transform the uri request to match in the server
    // std::string parsedUri = parseUri(request.getPath()); TO IMPLEMENT
    if (request.getMethod() == "GET")
        _getResponse();
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