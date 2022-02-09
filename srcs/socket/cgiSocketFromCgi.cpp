#include "cgiSocketFromCgi.hpp"


void        CgiSocketFromCgi::_checkHeaders(char c)
{
    if ((c == '\r') && ((_reader % 2) == 0))
        _reader++;
    else if ((c == '\n') && ((_reader % 2) == 1))
        _reader++;
    else
        _reader = 0;
}

/*
** set headers and return the status
*/
int        CgiSocketFromCgi::_constructCgiResponseHeaders(std::string& headerBuf)
{
    size_t      found, found2;
	std::string status;

    if (!(headerBuf.substr(0, 4) == "HTTP"))
    {
		if ((found = headerBuf.find("Status:")) != headerBuf.npos)
        {
            found2 = headerBuf.find("\r\n", found);
			status = headerBuf.substr(found + 7, found2 - found - 7);
			headerBuf = headerBuf.erase(found, 9 + status.length());
        }
        else
        {
            status = "200";
            headerBuf = headerBuf.erase(0, headerBuf.substr(0, headerBuf.find("\r\n") + 2).length());
        }
    }
    
    while (!headerBuf.empty())
    {
        if ((found = headerBuf.find(":")) != headerBuf.npos)
        {
            std::string key = headerBuf.substr(0, found);
            headerBuf = headerBuf.erase(0, found);
            std::string value = "";
            if ((found2 = headerBuf.find("\r\n")) != headerBuf.npos);
                value = headerBuf.substr(found, found2 + 2);
            _headers[key] = value;
        }
        else
            break ;
    }

    if ((_headers.find("Content-Length") == _headers.end()) && (_headers.find("Transfer-Encoding") == _headers.end()))
        _headers["Transfer-Encoding"] = "chunked";
    if (_headers.find("Content-Length") != _headers.end())
        _contentLengthPresent = true;

}

/*
** read the cgi response and parse et the same time
** because the cgi can send in one time or multiple
*/
void    CgiSocketFromCgi::readFromCgi()
{
    std::string                 cgiResponse;
    size_t                      bytes;
    std::vector<unsigned char>  buf(CGI_BUF_LEN + 1);
    Response                    response;

    _state = IN_PROGRESS;
    bytes = read(_pollFd.fd, &buf[0], CGI_BUF_LEN);
    std::cerr << "Return of the cgi : bytes to read : " << bytes << "\n";
    if (bytes > 0)
    {
        char c;
        for (size_t i = 0; i < bytes; i++)
        {
            c = buf[i];
            _buffer.push_back(buf[i]);
            if (_reader < 4)
            {
                _checkHeaders(c);
                _headerBuffer += c;
            }
            else
                cgiResponse += c;
            std::cerr << "buf[" << i << "] = " << static_cast<int>(buf[i]) << "\n";
            // if (buf[i] == 'z')
            //     _state = DONE;
        }
        if (_reader == 4) // just after the header
        {
            int status = _constructCgiResponseHeaders(_headerBuffer);
            _state += 1;
            if (_contentLengthPresent == false)
            {
                bytes = cgiResponse.length();
                cgiResponse = constructCgiResponseBodyChuncked(cgiResponse, bytes);
            }
            // -> set tous les headers trouves
            // response.setHeader();
            // -> set status
            // -> setContent
            // -> ajouter la reponse _client.push_back(response);
            _headerBuffer.clear();
        }
        else if (_reader > 4) 
        {
           
        }
    }
    std::cerr << "\n";
    
    // std::cerr << "buffer HEADER :\n" << _bufferHeaders << "\n";
    // std::cerr << "buffer BODY :\n" << _bufferBody << "\n";

    // PROBLEM IF NO BODY
    if (bytes < CGI_BUF_LEN && _reader == 4)
        _state = DONE;
    
}

/* GETTERS */
int             &CgiSocketFromCgi::getState()
{ return _state; }

std::string     CgiSocketFromCgi::getBuffer()
{ return std::string(_buffer.begin(), _buffer.end());}

ClientSocket*   CgiSocketFromCgi::getClient()
{ return _client; }

Request         &CgiSocketFromCgi::getRequest()
{ return _request; }

int             &CgiSocketFromCgi::getFdUseless()
{ return _fdUseless; }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
CgiSocketFromCgi::CgiSocketFromCgi(int fd[2], ClientSocket* client, Request request) : 
        Socket(),
        _state(INIT),
        _buffer(),
        _reader(0),
        _headerBuffer(),
        _headers(),
        _client(client),
        _request(request),
        _fdUseless(fd[1]),
        _contentLengthPresent(false)
{
    _type = cgiFrom;
    _pollFd.fd = fd[0];
    _pollFd.events = POLLIN;
}

CgiSocketFromCgi::~CgiSocketFromCgi() {}
