#include "hub.hpp"

void	Hub::start()
{
	_config.parse();
	_config.startSockets();
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
	int pollRet = 1;
	int acceptRet = 0;
		
	std::cout << "Waiting on poll() ...\n";

	// call poll and wait an infinite time
	// check to see if the poll call failed
	pollRet = poll(_config.getFds(), _config.getNfds(), -1);
	if (pollRet < 0)
	{
		std::cerr << "poll() failed\n";
		return ;
	}

	// check to see if the 3 minutes timeout expired
	if (pollRet == 0)
	{
		std::cerr << "poll() timeout\n";
		return ;
	}

	// one or more fd are readable. Need to determine which ones they are
	for (int i = 0; i < _config.getNfds(); i++)
	{
		// loop through to find the fd that returned POLLIN and determine whether it's the listening or the active connection
		if (_config.getFds()[i].revents == 0)
			continue;
		
		// if revent is not POLLIN, it's an inexpected result, log and end the server
		if (_config.getFds()[i].revents == POLLIN)
		{
			// if the current fd is one of our fds
			if (_isServerIndex(i))
			{
				// listening descriptor is readable
				std::cout << "Listening socket is readable\n";
				// accept all incoming connections that are queued up on the listening socket before
				// we loop back and call poll again
				while (acceptRet != -1)
				{
					// accept each incoming connection
					// if accept fails with EWOULDBLOCK, then we have accepted all of them.
					// Any other failure on accept will cause us to end the server
					acceptRet = accept(_config.getFds()[i].fd, NULL, NULL);
					if (acceptRet < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							std::cerr << "accept() failed\n";
							g_run = false;
						}
						break ;
					}

				// 	// add the new incoming connection to the pollfd structure
				// 	std::cout << "New incoming connection - " << acceptRet << "\n";
				// 	_config.getFds()[_config.getNfds()].fd = acceptRet;
				// 	_config.getFds()[_config.getNfds()].events = POLLIN;
				// 	_config.setNfds(_config.getNfds() + 1);
				// 	// loop back up and accept another incoming connection

				}
			}
			else
			{
				std::cerr << "it's not a server index = " << i << _config.getNfds() << _config.getTopClient() << " \n"; return ;
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