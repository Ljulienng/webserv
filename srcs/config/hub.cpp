#include "hub.hpp"

void	Hub::start()
{
	_config.parse();
	_config.startSockets();

	// std::map<std::string, Server>::iterator it = _config.getServers().begin();
	// for ( ; it != _config.getServers().end(); it++)
	// {
	// 	//class socketList qui contiendra toutes nos classes sockets
	// 	_socketList->addSocket(new Socket(it->second.getSockfd(), it->second.getSockaddr(), it->first));
	// }
}


bool Hub::_isServerIndex(int i)
{
	return (i < _config.getTopClient());
}

/*
** main loop
*/
void	Hub::process()
{
	// struct pollfd fds[200];
	// int timeout = 3 * 60 * 1000; // 3 minutes
	int rc = 1;
	// size_t nfds = 1;
	int new_sd = -1;
	bool end_server = false;
	
	// memset(fds, 0, sizeof(fds));
	// fds[0].fd = _config.getServers().begin()->second.getSockfd(); std::cout << "ERROR ...\n";
	// fds[0].events = POLLIN;
	
	while (end_server == false)
	{
		std::cout << "Waiting on poll() ...\n";

		// call poll and wait 3 minute for i
		
		// check to see if the poll call failed
		rc = poll(_config.getFds(), _config.getNfds(), -1);
		if (rc < 0)
		{
			std::cout << "poll() failed\n";
			break ;
		}

		// check to see if the 3 minutes timeout expired
		if (rc == 0)
		{
			std::cout << "poll() timeout\n";
			break ;
		}

		// one or more fd are readable. Need to determine which ones they are
		for (int i = 0; i < _config.getNfds(); i++)
		{
			// loop through to find the fd that returned POLLIN and determine whether it's the listening or the active connection
			if (_config.getFds()[i].revents == 0)
				continue;
			
			// if revent is not POLLIN, it's an inexpected result, log and end the server
			if (_config.getFds()[i].revents != POLLIN)
			{
				std::cout << "Error revents = " << _config.getFds()[i].revents << "\n";
				end_server = true;
				break ;
			}

			// if the current fd is one of our fds
			if (_isServerIndex(i))
			// if (_config.getFds()[i].fd == _config.getServers().begin()->second.getSockfd())
			{
				// listening descriptor is readable
				std::cout << "Listening socket is readable\n";

				// accept all incoming connections that are queued up on the listening socket before
				// we loop back and call poll again
				do {
					// accept each incoming connection
					// if accept fails with EWOULDBLOCK, then we have accepted all of them.
					// Any other failure on accept will cause us to end the server
					new_sd = accept(_config.getFds()[i].fd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							std::cout << "accept() failed\n";
							end_server = true;
						}
						break ;
					}

					// add the new incoming connection to the pollfd structure
					std::cout << "New incoming connection - " << new_sd << "\n";
					_config.getFds()[_config.getNfds()].fd = new_sd;
					_config.getFds()[_config.getNfds()].events = POLLIN;
					_config.setNfds(_config.getNfds() + 1);

					// loop back up and accept another incoming connection

				} while (new_sd != -1);
			}
		}
	}

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

Hub::~Hub()
{
}

Hub &Hub::operator=(const Hub &src)
{
	if (&src != this)
	{
		_config = src._config;
		// to be completed if new attributes
	}
	return (*this);
}