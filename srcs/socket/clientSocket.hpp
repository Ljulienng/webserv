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
        std::queue<Response>    _responses;
        std::string             _serverName;

    public :
        ClientSocket();
        ClientSocket(const ClientSocket &src);
        virtual ~ClientSocket();		
        ClientSocket    &operator=(const ClientSocket &src);
        
        int             verifBuffer();
        void            addRequest();
        void            addResponse(Response response);
        // SETTERS

        void                    setPort(unsigned short port);
        void                    setBuffer(std::string buffer);
        void                    setRequests(std::queue<Request> requests);
        void                    setServerName(std::string serverName);

        // GETTERS
        unsigned short          &getPort();
        std::string             &getBuffer();
        std::queue<Request>	    &getRequests();
        std::queue<Response>    &getResponses();
        std::string             &getServerName();

};

#endif