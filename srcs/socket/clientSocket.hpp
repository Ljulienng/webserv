#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

#include "socket.hpp"

class ClientSocket : public Socket
{
    private :
        unsigned short _port;
        // class Buffer

    public :
		ClientSocket();
		ClientSocket(const ClientSocket &src);
		virtual ~ClientSocket();		

		ClientSocket &operator=(const ClientSocket &src);

        // SETTERS
        void            setPort(unsigned short port);

        // GETTERS
        unsigned short     &getPort();
        

		
};

#endif