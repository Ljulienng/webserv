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
	int pollRet = 1;
	int acceptRet = 0;
		
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

				// PARSE THE REQUEST :
						// - push de la requete dans le SocketClient
				
				// PREPARE THE RESPONSE :
						// - recupere la derniere requete (top)
						// - verifie erreurs
						// - construit reponse
						// - push reponse dans le socketClient
						// - supprime requete(pop)
			}
		}
		else if (_config.getFds()[i].revents == POLLOUT)
		{
			// SEND RESPONSE :
						// - recupere la derniere reponse (top)
						// - on l'envoie et on la supprime (pop)
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