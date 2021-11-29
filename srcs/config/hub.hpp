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
		
    public :
		Hub();
		Hub(std::string configFile);
		Hub(const Hub &src);
		~Hub();		

		Hub &operator=(const Hub &src);

		void	_parseConfigFile();

		Configuration	&getConfig();
};

#endif