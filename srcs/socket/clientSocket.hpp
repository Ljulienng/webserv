#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

#include "socket.hpp"
#include "response.hpp"

class ClientSocket : public Socket
{
    private :
        unsigned short          _port;
        // class Buffer
        std::queue<Response>    _responses;

    public :
		ClientSocket();
		ClientSocket(const ClientSocket &src);
		virtual ~ClientSocket();		
		ClientSocket &operator=(const ClientSocket &src);

        void                    addResponse(Response response);

        // SETTERS
        void                    setPort(unsigned short port);

        // GETTERS
        unsigned short          &getPort();
        std::queue<Response>    &getResponses();
};

#endif