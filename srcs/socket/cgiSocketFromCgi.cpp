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
** read the cgi response and parse et the same time
** because the cgi can send in one time or multiple
*/
void    CgiSocketFromCgi::readFromCgi()
{
    size_t bytes;
    std::vector<unsigned char>  buf(CGI_BUF_LEN); //CGI_BUF_LEN

    _state = IN_PROGRESS;
    bytes = read(_pollFd.fd, &buf[0], CGI_BUF_LEN); //CGI_BUF_LEN
    // std::cerr << "Return of the cgi : bytes to read : " << bytes << "\n";
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
                _bufferHeaders += c;
            }
            else if (_reader == 4)
            {
                _reader++;
                _bufferBody += c;
            }
            else
                _bufferBody += c;
            // std::cerr << buf[i];
            // if (buf[i] == 'z')
            //     _state = DONE;
        }
        // if (_reader == 4) // just after the header
        // {
        //     if (bytes > 0)
        // }
        // else if (_reader > 4 && bytes < 10) 
        // {
        //     _state = DONE;
        // }
    }
    // std::cerr << "\n";

    std::cerr << "buffer HEADER :\n" << _bufferHeaders << "\n";
    std::cerr << "buffer BODY :\n" << _bufferBody << "\n";

    std::cerr << "bytes : " << bytes << "\n";
    
    // PROBLEM IF NO BODY
    if (bytes < CGI_BUF_LEN && _reader > 4) //CGI_BUF_LEN
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
        _bufferHeaders(),
        _bufferBody(),
        _client(client),
        _request(request),
        _fdUseless(fd[1])
{
    _type = cgiFrom;
    _pollFd.fd = fd[0];
    _pollFd.events = POLLIN;
}

CgiSocketFromCgi::~CgiSocketFromCgi() {}
