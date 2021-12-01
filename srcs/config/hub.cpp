#include "hub.hpp"

void	Hub::start()
{
	_config.parse();
	_config.startSockets();

	std::map<std::string, Server>::iterator it = _config.getServers().begin();
	for ( ; it != _config.getServers().end(); it++)
	{
		//class socketList qui contiendra toutes nos classes sockets
		_socketList->addSocket(new Socket(it->second.getSockfd(), it->second.getSockaddr(), it->first));
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

Hub::Hub() : _config(), _socketList(new SocketList())
			// to be completed if new attributes
{}

Hub::Hub(std::string configFile) : _config(configFile), _socketList(new SocketList()) {}

Hub::Hub(const Hub &src)
{
	*this = src;
}

Hub::~Hub()
{
	delete _socketList;
}

Hub &Hub::operator=(const Hub &src)
{
	if (&src != this)
	{
		_config = src._config;
		_socketList = src._socketList;
		// to be completed if new attributes
	}
	return (*this);
}