#include "cgiSocketFromCgi.hpp"


// void        CgiSocketFromCgi::_checkHeaders(char c)
// {
//     if ((c == '\r') && ((_reader % 2) == 0))
//         _reader++;
//     else if ((c == '\n') && ((_reader % 2) == 1))
//         _reader++;
//     else
//         _reader = 0;
// }

// bool     CgiSocketFromCgi::getTimeout()
// {
//     struct timespec now;
//     double          timeSinceStart = 0;
    
//     clock_gettime(CLOCK_MONOTONIC, &now);
//     timeSinceStart += (now.tv_sec - _startTime.tv_sec) * 1e9;
//     timeSinceStart += (now.tv_nsec - _startTime.tv_nsec) * 1e-9;
//     std::cerr << "timeSinceStart = " << timeSinceStart << "\n";
//     return timeSinceStart > 2;
// }

/*
** read the cgi response and parse et the same time
** because the cgi can send in one time or multiple
*/
void    CgiSocketFromCgi::readFromCgi()
{
    std::string                 cgiResponse;
    size_t                      bytes;
    std::vector<unsigned char>  buf(CGI_BUF_LEN + 1);

    _state = IN_PROGRESS;
    
    bytes = read(_pollFd.fd, &buf[0], CGI_BUF_LEN);
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
    clock_gettime(CLOCK_MONOTONIC, &_startTime);
    _type = cgiFrom;
    _pollFd.fd = fd[0];
    _pollFd.events = POLLIN;
}

CgiSocketFromCgi::~CgiSocketFromCgi() {}
