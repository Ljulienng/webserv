#ifndef HUB_HPP
# define HUB_HPP

#include "webserv.hpp"
#include "configuration.hpp"

extern bool    g_run;

/*
** general class to handle the program
*/

class Hub
{
    private :

		Configuration		_config;
		struct pollfd		_fds[MAX_CONNECTIONS];
		size_t				_nfds;

		void				_startSockets();
		void				_acceptIncomingConnections(size_t index);
		void				_receiveRequest(size_t index);
		void				_prepareResponse(size_t index);
		void 				_sendResponse(size_t index);
		void				_output(std::string msg, int fd);


    public :
	
		Hub();
		Hub(std::string configFile);
		Hub(const Hub &src);
		~Hub();		
		Hub &operator=(const Hub &src);

		void	start();
		void	process();

		// SETTERS
		void		setNfds(int nfds);

		// GETTERS
		Configuration		&getConfig();
		struct pollfd *		getFds();
		size_t				getNfds();
};



#endif