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
			std::cout << i <<  " POLLIN\n";
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
					client.setPort(_config.getServers().find(i)->second.getPort());
					// insert new client at index 'i + nb of servers' to find it easily
					_config.getClients().insert(std::pair<size_t, ClientSocket>(i + _config.getServers().size(), client));
					// add the new incoming connection to the pollfd structure
					std::cout << "New incoming connection - fd : " << acceptRet << "\n";
					std::cout << "nfds = " << _config.getNfds()<< "\n";
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
				/********** TEST A SUPPR AVANT PUSH ***************/
				std::vector<char>	bufRequest(2048);
				int bytesRequest = recv(_config.getFds()[i].fd, &bufRequest[0], 2048, 0);
				
				if (bytesRequest < 0)
					exit(EXIT_FAILURE);
				// else if (bytesRequest > 0)
				// {
				// 	for (int i = 0; i < bytesRequest; i++)
				// 		std::cout << bufRequest[i];
				// }
				/*************************************************/
					

				// PARSE THE REQUEST :
						// - push de la requete dans le SocketClient

						

				// PREPARE THE RESPONSE :
						// - recupere la derniere requete (top)
						// - verifie erreurs
						// - construit reponse
						// - push reponse dans le socketClient
						// - supprime requete(pop)
				/********** TEST A SUPPR AVANT PUSH ***************/
				Response resp;
				// resp.setContent("ENVOI DE LA REPONSE DANS LA SOCKET CLIENT !!!!!!!!!!!!!!!!!!!!!!");
				resp.setContent(std::string(bufRequest.begin(), bufRequest.end()));
				// std::cout << "nb of client= "<< _config.getClients().size() << " i=" << i << " SEG1\n";
				if (_config.getClients().find(i) != _config.getClients().end())
				{
					_config.getClients().find(i)->second.getResponses().push(resp);
					if (_config.getClients().find(i)->second.getResponses().empty() == false)
						_config.getFds()[i].events = POLLIN | POLLOUT;
				}
				/*************************************************/		
			}
		}
		else if (_config.getFds()[i].revents == POLLOUT)
		{
			std::cout << i <<  " POLLOUT\n";
			// SEND RESPONSE [ get responses queue of the client, send them and delete them ]
			
			std::map<size_t, ClientSocket>::iterator	client = _config.getClients().find(i);
			std::queue<Response>    					responses = client->second.getResponses();
			std::string									buffer/* = client->second.getBuffer()*/;

			// we process the responses one by one and append them to the client buffer
			// then delete the response until the queue is empty
			while (responses.empty() == false)
			{
				// process in FIFO order, we process the oldest element first
				Response response = responses.front();

				// then have to check header and status of the response
				// ...

				// put into a string the response
				std::string		message = response.getMessage();
				std::cout << "message to write in socket : \n" << message << "\n";
				// append it to the client buffer
				buffer.insert(buffer.end(), message.begin(), message.end());

				// delete the response
				responses.pop();
			}

			if (buffer.empty() == false)
			{
				// we can use send() ( without flag parameter, send is equivalent to write() )
				// so write into the _fds[i].fd the content of buffer
				// int bytes = send(_config.getFds()[i].fd, &buffer[0], buffer.size(), 0);
				// ou ...
				int bytes = write(_config.getFds()[i].fd, &buffer[0], buffer.size());

				if (bytes <= 0)
				{
					// disconnect each sockets (server and clients) and exit
					exit(EXIT_FAILURE);
				}
				// clear the buffer
				buffer.erase(buffer.begin(), buffer.begin() + bytes);
				// ok the response is sent !!!

				// we finished to write so we are now waiting for reading
				_config.getFds()[i].events = POLLIN;
			}
		
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