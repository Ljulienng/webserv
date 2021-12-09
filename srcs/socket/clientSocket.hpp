#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"

class ClientSocket : public Socket
{
    private :
        unsigned short          _port;
        std::string             _buffer;

        std::queue<Request>     _requests;
        // class Buffer
        std::queue<Response>    _responses;

    public :
		ClientSocket();
		ClientSocket(const ClientSocket &src);
		virtual ~ClientSocket();		

		ClientSocket    &operator=(const ClientSocket &src);
        void            addRequest();
        void                    addResponse(Response response);
        // SETTERS
        void            setPort(unsigned short port);
        void            setBuffer(std::string buffer);
        void            setRequests(std::queue<Request> requests);
        // GETTERS
        unsigned short      &getPort();
        std::string         &getBuffer();
        std::queue<Request>	&getRequests();
        std::queue<Response>    &getResponses();

};

#endif