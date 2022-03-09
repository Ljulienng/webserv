#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"

class ClientSocket : public Socket
{
    protected :
        unsigned short          _port;
        std::string             _buffer;
        std::list<Request>      _requests;
        std::list<Response*>    _responses;
        std::string             _serverName;
        struct timespec         _lastEvent;

    public :
        ClientSocket();
        ClientSocket(const ClientSocket &src);
        virtual ~ClientSocket();		
        ClientSocket    &operator=(const ClientSocket &src);

        void            addRequest();
        void            addResponse(Response* response);
        // SETTERS

        void                    setPort(unsigned short port);
        void                    setBuffer(std::string buffer);
        void                    setRequests(std::list<Request> requests);
        void                    setServerName(std::string serverName);
        void                    setTimeout();

        // GETTERS
        unsigned short          &getPort();
        std::string             &getBuffer();
        std::list<Request>	    &getRequests();
        std::list<Response*>    &getResponses();
        std::string             &getServerName();
        bool                    getTimeout();
};

#endif