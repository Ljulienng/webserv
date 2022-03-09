#ifndef HUB_HPP
# define HUB_HPP

#include "webserv.hpp"
#include "configuration.hpp"
#include "responseConstructor.hpp"
#include "log.hpp"
#include "configMatch.hpp"
#include "cgiSocketFromCgi.hpp"
#include "cgiSocketToCgi.hpp"
#include "cgiExecutor.hpp"
#include "str.hpp"

extern bool    	g_run;
extern std::vector<struct pollfd*>	g_fileArr;

class CgiSocket;
class ClientSocket;
class ListeningSocket;
class Server;

#define MAX_CGI_RUNNING 100

/*
** general class to handle the program
*/
class Hub
{
    private :

		struct pollfd		_fds[MAX_CONNECTIONS];
		size_t				_nfds;
		
		std::vector<ClientSocket*>			_clientSockets;
		std::vector<ListeningSocket*>		_listenSockets;
		std::vector<CgiSocketFromCgi*>		_cgiSocketsFromCgi;
		std::vector<CgiSocketToCgi*>		_cgiSocketsToCgi;
		std::vector<Socket*> 				_arr;		

		void				_addListeningSocket(Server& server);
		void				_addClientSocket(int acceptRet, Socket* socket);
		bool				_cgiClosed(size_t i, size_t *cgiCount);
		void				_storeFdToPoll();
		void				_checkTimeout();
		int					_waitPollEvent();
		bool				_acceptIncomingConnections(size_t i);
		bool				_receiveRequest(size_t i);
		void				_prepareResponse(size_t i);
		void				_prepareCgiResponse(size_t i);
		bool 				_sendResponse(size_t i);
		void 				_closeCgiConnections(ClientSocket* client);
		bool				_closeClientConnection(ClientSocket* client, size_t clientIndex);
		void				_closeConnection(size_t i, int type);
		void				_closeAllConnections();
		

    public :
	
		Hub();
		~Hub();		

		void	start(std::string configFile);
		void	process();
		void	clean();
		
};

#endif