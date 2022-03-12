#include "cgiSocketToCgi.hpp"

int    CgiSocketToCgi::writeToCgi()
{
    size_t  bytes;

    bytes = write(_pollFd.fd, _request.getBody().c_str(), _request.getBody().size());
    if (bytes < 0)
        return ERROR;
    else if (bytes == 0)
    {
        write(_pollFd.fd, "\n", 1);
        _pollFd.events = 0;
    }
    return GOOD;
}

int             &CgiSocketToCgi::getFdUseless()
{ return _fdUseless; }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
CgiSocketToCgi::CgiSocketToCgi(int fd[2], Request request) : 
        Socket(),
        _request(request),
        _fdUseless(fd[0])
{
    _type = cgiTo;
    _pollFd.fd = fd[1];
    _pollFd.events = POLLOUT;
}

CgiSocketToCgi::~CgiSocketToCgi()
{
    if (_pollFd.fd != 0)
    {
        close(_pollFd.fd);
        _pollFd.fd = 0;
    }
    if (_fdUseless != 0)
    {
        close(_fdUseless);
        _fdUseless = 0;
    }
}
