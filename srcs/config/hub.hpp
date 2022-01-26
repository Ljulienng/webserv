#ifndef HUB_HPP
# define HUB_HPP

#include "webserv.hpp"
#include "configuration.hpp"
#include "responseConstructor.hpp"
#include "log.hpp"

#define SERVER 0
#define CLIENT 1
extern bool    g_run;

/*
** general class to handle the program
*/

class Hub
{
    private :

		struct pollfd		_fds[MAX_CONNECTIONS];
		size_t				_nfds;

		void				_startSockets();
		void				_acceptIncomingConnections(size_t index);
		void				_receiveRequest(size_t index);
		void				_prepareResponse(size_t index);
		void 				_sendResponse(size_t index);
		void				_closeConnection(size_t index, int type);
		void				_closeAllConnections();
		bool				_isReadytoRead(size_t i);
		bool				_isError(size_t i);
		bool				_isReadyToWrite(size_t i);

		Hub();

    public :
	
		Hub(std::string configFile);
		Hub(const Hub &src);
		~Hub();		
		Hub &operator=(const Hub &src);

		void	start();
		void	process();

		// SETTERS
		void		setNfds(int nfds);

		// GETTERS
		struct pollfd 		*getFds();
		size_t				getNfds();
};

#endif