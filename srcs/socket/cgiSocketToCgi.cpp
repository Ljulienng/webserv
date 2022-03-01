#include "cgiSocketToCgi.hpp"

void    CgiSocketToCgi::writeToCgi()
{
    size_t  bytes;

    // std::cerr << "write to Cgi = \n";
    bytes = write(_pollFd.fd, _request.getBody().c_str(), _request.getBody().size());
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

CgiSocketToCgi::~CgiSocketToCgi(){}
