#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"

class ListeningSocket : public Socket
{
    public:

        enum errors
        {
            NONE,
            NONBLOCK,
            SETSOCKOPT,
            BIND,
            LISTEN
        };
    
    
    private :

        int     _state;
        ListeningSocket();


    public :
        
        ListeningSocket(std::string serverName);
        ListeningSocket(const ListeningSocket &src);
        virtual ~ListeningSocket();		
        ListeningSocket    &operator=(const ListeningSocket &src);
      
        int 	start(std::string ip, unsigned short port);
		int		closeSocket();
		int		bindSocket();
		int		setSocketOptions();
		int		setNonBlock();
		int		listenSocket();
};

#endif