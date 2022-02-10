#ifndef CGISOCKETFROMCGI_HPP
# define CGISOCKETFROMCGI_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"
#include "clientSocket.hpp"

#define CGI_BUF_LEN 50

enum state
{
    INIT,
    IN_PROGRESS,
    HEADERS_DONE,
    DONE
};

class CgiSocketFromCgi : public Socket
{
    protected :
        int                                 _state;
        std::vector<unsigned char>          _buffer;
        int                                 _reader;
        std::string                         _headerBuffer;
        std::map<std::string, std::string>  _headers;
        ClientSocket*                       _client;
        Request                             _request;
        int                                 _fdUseless;
      

    public :

        CgiSocketFromCgi(int fd[2], ClientSocket* client, Request request);
        virtual ~CgiSocketFromCgi();	

        void            readFromCgi();

        int             &getState();
        std::string     getBuffer();
        ClientSocket*   getClient();
        Request         &getRequest();
        int             &getFdUseless();

};

#endif