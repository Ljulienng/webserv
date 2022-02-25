#include "cgiSocketFromCgi.hpp"

/*
** read the cgi response and parse et the same time
** because the cgi can send in one time or multiple
*/
void    CgiSocketFromCgi::readFromCgi()
{
    std::string                 cgiResponse;
    size_t                      bytes;
    std::vector<unsigned char>  buf(300000);

    bytes = read(_pollFd.fd, &buf[0], 300000);
    std::cerr << "Return of the cgi : bytes to read : " << bytes << "\n";
    if (bytes > 0)
    {
        for (size_t i = 0; i < bytes; i++)
        {
            _buffer.push_back(buf[i]);
            std::cerr << buf[i];
        }
        std::cerr << "\n";
    }   
}

/* GETTERS */
std::string     CgiSocketFromCgi::getBuffer()
{ return std::string(_buffer.begin(), _buffer.end());}

ClientSocket&   CgiSocketFromCgi::getClient()
{ return _client; }

Request         &CgiSocketFromCgi::getRequest()
{ return _request; }

int             &CgiSocketFromCgi::getFdUseless()
{ return _fdUseless; }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
CgiSocketFromCgi::CgiSocketFromCgi(int fd[2], ClientSocket& client, Request request) : 
        Socket(),
        _buffer(),
        _reader(0),
        _headerBuffer(),
        _headers(),
        _client(client),
        _request(request),
        _fdUseless(fd[1])
{
    _type = cgiFrom;
    _pollFd.fd = fd[0];
    _pollFd.events = POLLIN;
}

CgiSocketFromCgi::~CgiSocketFromCgi() {}
