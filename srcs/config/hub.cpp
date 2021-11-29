#include "hub.hpp"

void	Hub::start()
{
	_config.parse();
	_config.startSockets();

	// store fd of each server in fdSet
	long fd;
	std::map<std::string, Server>::iterator it = _config.getServers().begin();
	for ( ; it != _config.getServers().end(); it++)
	{
		fd = (*it).second.getSockfd();
		FD_SET(fd, &_fdSet);
	}
}

void	Hub::process()
{

}

/* GETTERS */

Configuration	&Hub::getConfig()
{
	return _config;
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