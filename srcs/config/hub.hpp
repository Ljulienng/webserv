#ifndef HUB_HPP
# define HUB_HPP

#include "webserv.hpp"
#include "configuration.hpp"
// #include "socketList.hpp"
extern bool    g_run;
/*
** general class to handle the program
*/
class Hub
{
    private :
		Configuration		_config;

		void				_acceptIncomingConnections(size_t index);
		void				_prepareResponse(size_t index);
		void 				_sendResponse(size_t index);

    public :
		Hub();
		Hub(std::string configFile);
		Hub(const Hub &src);
		~Hub();		

		Hub &operator=(const Hub &src);

		void	start();
		void	process();

		Configuration	&getConfig();
};

#endif