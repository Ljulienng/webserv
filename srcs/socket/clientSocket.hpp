#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

#include "socket.hpp"
#include "request.hpp"

class ClientSocket : public Socket
{
    private :
        unsigned short          _port;
        std::string             _buffer;

        std::queue<Request>     _requests;

    public :
		ClientSocket();
		ClientSocket(const ClientSocket &src);
		virtual ~ClientSocket();		

		ClientSocket    &operator=(const ClientSocket &src);
        void            addRequest();
        // SETTERS
        void            setPort(unsigned short port);
        void            setBuffer(std::string buffer);
        void            setRequests(std::queue<Request> requests);
        // GETTERS
        unsigned short      &getPort();
        std::string         &getBuffer();
        std::queue<Request>	&getRequests();
		
};

#endif