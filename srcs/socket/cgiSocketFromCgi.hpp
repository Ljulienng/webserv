#ifndef CGISOCKETFROMCGI_HPP
# define CGISOCKETFROMCGI_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"
#include "clientSocket.hpp"

enum state
{
    INIT,
    INPROGRESS,
    DONE
};

class CgiSocketFromCgi : public Socket
{
    protected :
        int                         _state;
        std::vector<unsigned char>  _buffer;
        ClientSocket*               _client;
        Request                     _request;

    public :
        CgiSocketFromCgi(int fd[2], ClientSocket* client, Request request);
        virtual ~CgiSocketFromCgi();	

        void    readFromCgi();

        int             &getState();
        std::string     getBuffer();
        ClientSocket*   getClient();
        Request         &getRequest();

};

#endif