#include "hub.hpp"

void	Hub::_parseConfigFile()
{
	_config.parse();
	_config.debug();
}


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

Hub::Hub() : _config()
			// to be completed if new attributes
{}

Hub::Hub(std::string configFile) : _config(configFile) {}

Hub::Hub(const Hub &src)
{
	*this = src;
}

Hub::~Hub() {}

Hub &Hub::operator=(const Hub &src)
{
	if (&src != this)
	{
		_config = src._config;
		// to be completed if new attributes
	}
	return (*this);
}