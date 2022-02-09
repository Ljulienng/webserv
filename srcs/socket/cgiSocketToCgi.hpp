#ifndef CGISOCKETTOCGI_HPP
# define CGISOCKETTOCGI_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"


class CgiSocketToCgi : public Socket
{
    protected :
        Request _request;
        int     _fdUseless;
    

    public :
        CgiSocketToCgi(int fd[2], Request request);
        virtual ~CgiSocketToCgi();

        void            writeToCgi();
        int             &getFdUseless();
};

#endif