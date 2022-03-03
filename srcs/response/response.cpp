#include "response.hpp"

std::vector<struct pollfd*>	g_fileArr;

void    Response::_updateMessage()
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
    _message += std::string(_content.begin(), _content.end());
}


void                Response::addMultipart(t_multipart* part)
{
    _multiparts.push_back(part);
}

void    Response::readFile(bool *endOfResponse, bool *endToReadFile)
{
    char bufFile[BUF_SIZE];
    size_t bytes = read(_pollFdFile.fd, bufFile, BUF_SIZE);
    if (bytes > 0)
        for (size_t i = 0; i < bytes; i++)
            _content.push_back(bufFile[i]);
    else
    {
        *endOfResponse = true;
        *endToReadFile = true;
    }
}

void    Response::writeFile(bool *endOfResponse, bool *endToWriteFile)
{
    if (_isMultipart)
    {
        if (!_multiparts.empty())
        {
            std::list<t_multipart*>::iterator it = _multiparts.begin();
            write(_pollFdFile.fd, &(*it)->content[0], (*it)->length);
            delete *it;
            _multiparts.erase(it);

        }
        if (_multiparts.empty())
        {
            *endOfResponse = true;
            *endToWriteFile = true;
        }
    }
    else
    {
        write(_pollFdFile.fd, &_contentToCopyInFile[0], _contentToCopyInFile.length());
        *endOfResponse = true;
        *endToWriteFile = true;
    } 
}

void 	Response::addFile()
{
    g_fileArr.push_back(&_pollFdFile);
}

void 	Response::deleteFile()
{
    std::vector<struct pollfd *>::iterator it = g_fileArr.begin();
    while (*it != &_pollFdFile)
        it++;
    if (*it == &_pollFdFile)
        g_fileArr.erase(it);
}

void                Response::endToReadorWrite() 
{
    close(_pollFdFile.fd);
    _pollFdFile.fd = 0;
    _pollFdFile.events = 0;
    _indexFile = -1;
    _stateFile = NONE;
    deleteFile();
    setContentLength();
}

/* SETTERS */
void    Response::setHeader(std::string key, std::string value)
{
    _headers.insert(std::pair<std::string, std::string>(key, value));
}

void    Response::setContent(std::vector<unsigned char> content, std::string contentType)
{
    _content = content;
    setHeader("Content-Type", contentType);
    setHeader("Content-Length", myItoa(content.size()));
}

void    Response::setContentType(std::string contentType)
{
    setHeader("Content-Type", contentType);
}

void    Response::setContentLength()
{
    setHeader("Content-Length", myItoa(_content.size()));
}

void    Response::setStatus(int status)
{
    _httpStatus.setStatus(status);
}

void    Response::setIndexFile(int indexFile)
{
    _indexFile = indexFile;
}

bool    Response::setPollFdFileToRead(const char *file)
{
    _pollFdFile.fd = open(file, O_RDONLY);
    if (_pollFdFile.fd < 0)
        return false;
    _pollFdFile.events = POLLIN;
    _stateFile = DATATOREAD;
    return true;
}

bool    Response::setPollFdFileToWrite(std::string file, bool multipart, std::string content)
{
    _pollFdFile.fd = open(file.c_str(), O_CREAT | O_WRONLY | O_TRUNC , 0666);
    if (_pollFdFile.fd < 0)
        return false;
    _pollFdFile.events = POLLOUT;
    _isMultipart = multipart;
    _contentToCopyInFile = content;
    _stateFile = DATATOWRITE;
    return true;
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

std::string     &Response::getHttpVersion()
{ return  _httpVersion; }

HttpStatus      &Response::getHttpStatus()
{ return _httpStatus; }

std::vector<unsigned char>     &Response::getContent()
{ return  _content; }

std::string     &Response::getMessage()
{
    _updateMessage();
    return  _message;
}

std::list<t_multipart*> &Response::getMultiparts()
{ return _multiparts; }

struct pollfd       &Response::getPollFdFile()
{ return _pollFdFile; }

int   &Response::getStateFile()
{ return _stateFile; }

int  &Response::getIndexFile()
{ return _indexFile; }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Response::Response() : 
        _headers(),
        _httpVersion("HTTP/1.1"),
        _httpStatus(),
        _content(),
        _message(),
        _pollFdFile(),
        _stateFile(NONE),
        _indexFile(-1),
        _multiparts(),
        _isMultipart(false),
        _contentToCopyInFile()
{
    setHeader("Server", "Webserv_42");
    setHeader("Date", log::getTimestamp());
    setHeader("Connection", "keep-alive");
}

Response::Response(const Response &src)
{
	*this = src;
}

Response::~Response()
{
    std::list<t_multipart*>::iterator it = _multiparts.begin();
    for ( ; it != _multiparts.end(); it++)
        delete *it;

    if (_pollFdFile.fd != 0)
        deleteFile();
}

Response    &Response::operator=(const Response &src)
{
	if (&src != this)
	{
		_headers = src._headers;
        _httpVersion = src._httpVersion;
        _httpStatus = src._httpStatus;
        _content = src._content;
        _message = src._message;
        _pollFdFile = src._pollFdFile;
        _stateFile = src._stateFile;
        _indexFile = src._indexFile;
        _multiparts = src._multiparts;
        _isMultipart = src._isMultipart;
        _contentToCopyInFile = src._contentToCopyInFile;
		// to be completed if new attributes
	}
	return (*this);
}