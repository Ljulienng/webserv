#ifndef CGISOCKETTOCGI_HPP
# define CGISOCKETTOCGI_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"


class CgiSocketToCgi : public Socket
{
    protected :
        Request _request;
    

    public :
        CgiSocketToCgi(int fd, Request request);
        virtual ~CgiSocketToCgi();

        void    writeToCgi();
};

#endif