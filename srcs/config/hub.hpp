#ifndef HUB_HPP
# define HUB_HPP

#include "configuration/configuration.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <exception>

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

};

#endif