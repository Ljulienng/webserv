#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

#include "socket.hpp"

class ClientSocket : public Socket
{
    private :
        unsigned short      _port;
        std::string         _buffer;

    public :
		ClientSocket();
		ClientSocket(const ClientSocket &src);
		virtual ~ClientSocket();		

		ClientSocket &operator=(const ClientSocket &src);

        // SETTERS
        void            setPort(unsigned short port);
        void            setBuffer(std::string buffer);

        // GETTERS
        unsigned short      &getPort();
        std::string         &getBuffer();

        

		
};

#endif