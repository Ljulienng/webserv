#ifndef HUB_HPP
# define HUB_HPP

#include "webserv.hpp"
#include "configuration.hpp"

/*
** general class to handle the program
*/
class Hub
{
    private :
		Configuration		_config;
		fd_set				_fdSet;	// all fd get thanks to socket()
		
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