#ifndef LISTENINGSOCKET_HPP
# define LISTENINGSOCKET_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"

class ListeningSocket : public Socket
{
    protected :
    

    public :
        ListeningSocket();
        ListeningSocket(std::string serverName);
        ListeningSocket(const ListeningSocket &src);
        virtual ~ListeningSocket();		
        ListeningSocket    &operator=(const ListeningSocket &src);
      
        void 		start(std::string ip, unsigned short port);
        void		createSocket();
		void		closeSocket();
		void		bindSocket();
		void		setSocketOptions();
		void		setNonBlock();
		void		listenSocket();
};

#endif