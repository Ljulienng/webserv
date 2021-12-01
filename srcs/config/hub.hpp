#ifndef HUB_HPP
# define HUB_HPP

#include "webserv.hpp"
#include "configuration.hpp"
// #include "socketList.hpp"

/*
** general class to handle the program
*/
class Hub
{
    private :
		Configuration		_config;
		// SocketList *		_socketList;


		bool _isServerIndex(int i);

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