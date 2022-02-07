#include "cgiSocketFromCgi.hpp"

void    CgiSocketFromCgi::readFromCgi()
{
    size_t bytes;
    std::vector<unsigned char>  buf(MAX_BUF_LEN);

    _state = INPROGRESS;
    bytes = read(_pollFd.fd, &buf[0], MAX_BUF_LEN);
    // std::cerr << "Return of the cgi : bytes to read : " << bytes << "\n";
    if (bytes > 0)
    {
        for (size_t i = 0; i < bytes; i++)
        {
            _buffer.push_back(buf[i]);
            // std::cerr << buf[i];
        }
    }
    // std::cerr << "\n";
    if (bytes < MAX_BUF_LEN)
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

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
CgiSocketFromCgi::CgiSocketFromCgi(int fd[2], ClientSocket* client, Request request) : 
        Socket(),
        _state(INIT),
        _buffer(),
        _client(client),
        _request(request)
{
    _type = cgiFrom;
    _pollFd.fd = fd[0];
    _pollFd.events = POLLIN;
}

CgiSocketFromCgi::~CgiSocketFromCgi() {}
