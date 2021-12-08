#include "hub.hpp"

void	Hub::start()
{
	_config.parse();
	_config.startSockets();
}

/*
** main loop
*/
void	Hub::process()
{
	int 				pollRet = 1;
	int 				acceptRet = 0;
	// size_t				j = 0;
		
	// call poll and wait an infinite time

	pollRet = poll(_config.getFds(), _config.getNfds(), -1);
	if (pollRet < 0) // poll() failed
		return ;
	// one or more fd are readable. Need to determine which ones they are
	for (size_t i = 0; i < _config.getNfds(); i++)
	{
		// loop through to find the fd that returned POLLIN and determine whether it's the listening or the active connection
		if (_config.getFds()[i].revents == 0)
			continue;
		
		// if revent is POLLIN, there is data waiting to be read
		if (_config.getFds()[i].revents == POLLIN)
		{
			// if the current fd is one of our servers, we connect a new client
			// listening descriptor is readable
			if (i < _config.getServers().size()) // fd stored after "nb of servers" are clients fd and not servers
			{
				// accept all incoming connections that are queued up on the listening socket before
				// accept each incoming connection
				std::cout << "i =  " << i << " size = " << _config.getServers().size() << std::endl;
				while (42)
				{
					acceptRet = accept(_config.getFds()[i].fd, NULL, NULL);
					if (acceptRet == -1) // no connection is present in the queue
						break ;
					ClientSocket client;
					client.setFd(acceptRet);
					client.setPort(_config.getServers()[i].getPort());
					_config.getClients().insert(std::pair<size_t, ClientSocket>(i, client));
					// add the new incoming connection to the pollfd structure
					std::cout << "New incoming connection - fd : " << acceptRet << "\n";
					_config.getFds()[_config.getNfds()].fd = acceptRet;
					_config.getFds()[_config.getNfds()].events = POLLIN;
					_config.setNfds(_config.getNfds() + 1);
					// loop back up and accept another incoming connection
				}
			}
			// it's not a listening socket (server), therefore an existing connection must be readable (client)
			// it's a client so we need to receive the request (recv), parse it and prepare response
			else
			{
				// RECEIVE THE REQUEST (recv)
				int 				bytes = 0;
				std::vector<char>	buffer(MAX_BUF_LEN);


				while (bytes == MAX_BUF_LEN) // Loop to get all the data into the buffer MAX_BUF_LEN at a time
				{
					bytes = recv(_config.getFds()[i].fd, &buffer[0], MAX_BUF_LEN, 0);
					if (bytes == -1)
						throw std::string("Error: can't receive client request");
					else
						_config.getClients()[i].getBuffer().append(buffer.begin(), buffer.end());
				}
				// std::cout << _config.getClients()[j].getBuffer() << std::endl;
				for (int i = 0; i < bytes; i++)
					std::cout << buffer[i];
				buffer.clear();
				bytes = 0;
				// std::cout << "Went in else" << std::endl;
				// // RECEIVE THE REQUEST (recv)
				// int 				bytes = 0;
				// std::vector<char>	buffer(MAX_BUF_LEN);

				// bytes = recv(_config.getFds()[i].fd, &buffer[0], MAX_BUF_LEN, 0);
				// if (bytes < 0)
				// 	throw std::string("Error: can't receive client request");
				// else if (bytes > 0)
				// {
				// 	for (int i = 0; i < bytes; i++)
				// 		std::cout << buffer[i];
				// 	std::cout << std::endl;
				// }
				// else
				// 	bytes = 0;
				// PARSE THE REQUEST
				// PREPARE THE RESPONSE
			}
		}
		else if (_config.getFds()[i].revents == POLLOUT)
		{
			// SEND RESPONSE
		}
		else
		{
			// ERROR QUIT PROGRAM
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
{}

Hub &Hub::operator=(const Hub &src)
{
	if (&src != this)
	{
		_config = src._config;
		// to be completed if new attributes
	}
	return (*this);
}