#ifndef CGISOCKETFROMCGI_HPP
# define CGISOCKETFROMCGI_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"
#include "clientSocket.hpp"


class CgiSocketFromCgi : public Socket
{
    protected :
        std::vector<unsigned char>          _buffer;
        int                                 _reader;
        std::string                         _headerBuffer;
        std::map<std::string, std::string>  _headers;
        ClientSocket&                       _client;
        Request                             _request;
        int                                 _fdUseless;
      

    public :

        CgiSocketFromCgi(int fd[2], ClientSocket& client, Request request);
        virtual ~CgiSocketFromCgi();	

        void            readFromCgi();

        std::string     getBuffer();
        ClientSocket&   getClient();
        Request         &getRequest();
        int             &getFdUseless();

};

#endif