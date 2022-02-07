#include "cgiSocketToCgi.hpp"

void    CgiSocketToCgi::writeToCgi()
{
    size_t  bytes;

    // std::cerr << "write to Cgi = \n";
	// std::cerr << _request.getBody() << "\n";
    bytes = write(_pollFd.fd, _request.getBody().c_str(), _request.getBody().size());
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
CgiSocketToCgi::CgiSocketToCgi(int fd, Request request) : 
        Socket(),
        _request(request)
{
    _type = cgiTo;
    _pollFd.fd = fd;
    _pollFd.events = POLLOUT;
}

CgiSocketToCgi::~CgiSocketToCgi() {}
