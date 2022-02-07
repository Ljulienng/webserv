#include "hub.hpp"

/*
** parse configuration file
** start socket for each server of the configuration file
*/
void	Hub::start()
{
	Configuration::getInstance().parse();
	
	std::vector<Server> &servers = Configuration::getInstance().getServers();
	for (size_t i = 0; i < servers.size(); i++)
		_addListeningSocket(servers[i]);
}

void	Hub::_addListeningSocket(Server &server)
{
	ListeningSocket* newListenSocket = new ListeningSocket(server.getName());
	struct pollfd newPollFd;

	newPollFd.fd = socket(AF_INET, SOCK_STREAM, 0);;
	newPollFd.events = POLLIN;
	newListenSocket->setPollFd(newPollFd);
	newListenSocket->start(server.getIp(), server.getPort());
	_listenSockets.push_back(newListenSocket);
}

void	Hub::_addClientSocket(int acceptRet, Socket* listenSocket)
{
	ClientSocket* newClientSocket = new ClientSocket();
	struct pollfd newPollFd;
	newClientSocket->setServerName(listenSocket->getServerName());
	newClientSocket->setFd(acceptRet);
	newClientSocket->setPort(listenSocket->getAddr().sin_port);
	newPollFd.fd = acceptRet;
	newPollFd.events = POLLIN;
	newClientSocket->setPollFd(newPollFd);
	_clientSockets.push_back(newClientSocket);
}

void	Hub::_storeFdToPoll()
{
	memset(_fds, 0, sizeof(_fds));
	_nfds = 0;
	_arr.clear();
	for (std::vector<ListeningSocket*>::iterator it = _listenSockets.begin(); it != _listenSockets.end(); it++, _nfds++)
	{
		_fds[_nfds] = (*it)->getPollFd();
		_arr.push_back(*it);
		_arr[_nfds]->_index = _nfds;
	}
	for (std::vector<ClientSocket*>::iterator it = _clientSockets.begin(); it != _clientSockets.end(); it++, _nfds++)
	{
		_fds[_nfds] = (*it)->getPollFd();
		_arr.push_back(*it);
		_arr[_nfds]->_index = _nfds - _listenSockets.size();
	}
	for (std::vector<CgiSocketFromCgi*>::iterator it = _cgiSocketsFromCgi.begin(); it != _cgiSocketsFromCgi.end(); it++, _nfds++)
	{ 
		_fds[_nfds] = (*it)->getPollFd();
		_arr.push_back(*it);
		_arr[_nfds]->_index = _nfds - _listenSockets.size() - _clientSockets.size();
	}
	for (std::vector<CgiSocketToCgi*>::iterator it = _cgiSocketsToCgi.begin(); it != _cgiSocketsToCgi.end(); it++, _nfds++)
	{
		_fds[_nfds] = (*it)->getPollFd();
		_arr.push_back(*it);
		_arr[_nfds]->_index = _nfds - _listenSockets.size() - _clientSockets.size() - _cgiSocketsFromCgi.size();
	}
}

void	Hub::process()
{
	_storeFdToPoll();
	// std::cerr << "Before poll() -> nfds = " << _nfds << "  size of _sockArr = " << _arr.size() << "\n";
	int pollRet = poll(_fds, _nfds, -1); // call poll and wait an infinite time
	if (pollRet < 0) // poll failed or SIGINT received [poll is a blocking function and SIGINT will unblock it]
	{	
		_closeAllConnections();
		return ;
	}
	for (size_t i = 0; i < _nfds; i++)
	{
		if (_fds[i].revents == 0)
			continue;
		else if ((_fds[i].revents & POLLIN) == POLLIN)
		{
			if (_arr[i]->getType() == Socket::server)
			{
				_acceptIncomingConnections(i);
			}
			else if (_arr[i]->getType() == Socket::client)
			{
				_receiveRequest(i);
				_prepareResponse(i);
			}
			else if (_arr[i]->getType() == Socket::cgiFrom)
			{
				_cgiSocketsFromCgi[_arr[i]->_index]->readFromCgi();
				if (_cgiSocketsFromCgi[_arr[i]->_index]->getState() == DONE)
				{
					_prepareCgiResponse(_arr[i]->_index);
					//close both connections at the same time
					_closeConnection(_arr[i]->_index, Socket::cgiTo); // find a way to match the ToCgi index with the 
					_closeConnection(_arr[i]->_index, _arr[i]->getType());
				}
			}
		}
		else if ((_fds[i].revents & POLLOUT) == POLLOUT)
		{
			if (_arr[i]->getType() == Socket::client)
				_sendResponse(i);
			else if (_arr[i]->getType() == Socket::cgiTo)
				_cgiSocketsToCgi[_arr[i]->_index]->writeToCgi();
		}
		else if ((_fds[i].revents & POLLERR) == POLLERR || (_fds[i].revents & POLLHUP) == POLLHUP)
			_closeConnection(_arr[i]->_index, _arr[i]->getType());

		else
		{
			_closeAllConnections();
			exit(EXIT_FAILURE);
		}
	}
}

/*
** accept client :
**   - accept all incoming connections on the listening socket (server host:port)
**   - create a client
**   - increment the nb of fds with the return of accept (fd of the new socketClient)
*/
void		Hub::_acceptIncomingConnections(size_t i)
{
	while (1)
	{
		if (_clientSockets.size() == MAX_CONNECTIONS)
			_closeConnection(0, _arr[i]->getType()); // disconnect the first client
		int acceptRet = accept(_arr[i]->getPollFd().fd, NULL, NULL);

		if (acceptRet == -1) // no connection is present
			break ;

		_addClientSocket(acceptRet, _arr[i]);

		// add the new incoming connection to the pollfd structure
		log::logEvent("New incoming connection", acceptRet);

		// loop back up and accept another incoming connection
	}
}

/*
** receive and parse the request
*/
void		Hub::_receiveRequest(size_t i)
{
	int 				bytes = 0;
	std::vector<char>	buffer(MAX_BUF_LEN);
	ClientSocket* 		client = _clientSockets[_arr[i]->_index];

	bytes = recv(client->getPollFd().fd, &buffer[0], MAX_BUF_LEN, 0);
	if (bytes < 0)
	{
		_closeAllConnections();
		exit(EXIT_FAILURE);
	}
	else if (bytes > 0)
	{
		// std::cerr << "bytes read = " << bytes << "\n";
		client->getBuffer().append(buffer.begin(), buffer.end());
		if (bytes < MAX_BUF_LEN)
		{
			// std::cerr << "buffer = \n" << client->getBuffer() << "\n";
			client->addRequest();
			log::logEvent("Received a new request", client->getPollFd().fd);
		}
	}
	else //bytes = 0;
	{
		std::cerr << "Need to close connection bytes read = 0\n";
		_closeConnection(_arr[i]->_index, _arr[i]->getType()); // disconnect the client
	}
}

static bool _needCgi(Request request, t_configMatch configMatch)
{
	return (configMatch.server.getCgi().first == ".php"
			&& request.getPath().find(configMatch.server.getCgi().first) != std::string::npos
			&& (request.getMethod() == "GET" || request.getMethod() == "POST"));
}
/*
** prepare response :
**	- get the requests of the client
**  - loop to get requests one by one
**  - check errors
**  - build the response
**  - add the response in socketClient
**  - delete the request
*/
void		Hub::_prepareResponse(size_t i)
{
	ClientSocket* 			client = _clientSockets[_arr[i]->_index];
	std::list<Request>		&requests = client->getRequests();

	while (requests.empty() == false)
	{
		std::list<Request>::iterator it = client->getRequests().begin();
		for (; it != client->getRequests().end(); it++)
		{
			// FIRST : need to check if error handled in request
			
			t_configMatch configMatch = getConfigMatch(*it, client->getServerName());
			
			if (_needCgi(*it, configMatch))
			{	// execute cgi and create 2 cgi sockets (in and out)
				// copie the request to have an empty pool of request and leave the loop
				// store the client to send request at the end of reading
				CgiExecutor cgi(*it, client, configMatch);
				cgi.execCgi();
				_cgiSocketsFromCgi.push_back(cgi.getCgiSocketFromCgi());
				_cgiSocketsToCgi.push_back(cgi.getCgiSocketToCgi());
			}
			else
			{
				Response 	response;
				response = constructResponse(*it, client->getServerName());
				client->getResponses().push_back(response);
			}
			requests.erase(it++);
		}
	}

	// the socket is now ready to write in addition to reading because we have added a response
	if (client->getResponses().empty() == false)
		client->getPollFd().events = POLLIN | POLLOUT;
}

void		Hub::_prepareCgiResponse(size_t i)
{
	Response response;
	t_configMatch configMatch;
	
	configMatch = getConfigMatch(_cgiSocketsFromCgi[i]->getRequest(), _cgiSocketsFromCgi[i]->getClient()->getServerName());
	response = newCgiResponse(_cgiSocketsFromCgi[i]->getBuffer(), response, configMatch);
	
	_cgiSocketsFromCgi[i]->getClient()->getResponses().push_back(response);
	_cgiSocketsFromCgi[i]->getClient()->getPollFd().events = POLLIN | POLLOUT;
}

/*
** send response :
**	- get the responses of the client
**  - loop to get responses one by one
**  - check errors
**  - extract elements of the responses and get into a string
**  - append each response in a buffer
**  - write/send in the buffer the response 
*/
void 		Hub::_sendResponse(size_t i)
{
	ClientSocket* 			client = _clientSockets[i - _listenSockets.size()];
	std::list<Response>		&responses = client->getResponses();
	std::string				buffer = client->getBuffer();

	// we process the responses one by one and append them to the client buffer
	// then delete the response until the queue is empty
	while (responses.empty() == false)
	{
		Response response = responses.front(); // process in FIFO order, we process the oldest element first

		// then have to check header and status of the request
		// ...

		std::string		message = response.getMessage(); // put into a string the response
		buffer.insert(buffer.end(), message.begin(), message.end()); // append it to the client buffer
		log::logEvent("Response sent", client->getPollFd().fd);
		responses.pop_front();
	}

	if (buffer.empty() == false)
	{
		// we can use send() ( without flag parameter, send is equivalent to write() )
		// so write into the _fds[i].fd the content of buffer
		int bytes = write(client->getPollFd().fd, &buffer[0], buffer.size());

		if (bytes <= 0)
		{
			_closeAllConnections();
			exit(EXIT_FAILURE);
		}
		buffer.erase(buffer.begin(), buffer.begin() + bytes); // clear the buffer
	}

	if (buffer.empty()) // finished to write so we are now waiting for reading
		client->getPollFd().events = POLLIN;
}

void		Hub::_closeConnection(size_t i, int type)
{
		std::cerr << "Connection closed ";
		if (type == Socket::server)
		{
			std::cerr << "[server : fd " << _listenSockets[i]->getPollFd().fd << "] \n";
			close(_listenSockets[i]->getPollFd().fd);
			_listenSockets.erase(_listenSockets.begin() + i);
		}
		else if (type == Socket::client)
		{
			std::cerr << "[client : fd " << _clientSockets[i]->getPollFd().fd << "] \n";
			close(_clientSockets[i]->getPollFd().fd);
			_clientSockets.erase(_clientSockets.begin() + i);
		}
		else if (type == Socket::cgiFrom)
		{
			std::cerr << "[cgi From : fd " << _cgiSocketsFromCgi[i]->getPollFd().fd << "] \n";
			close(_cgiSocketsFromCgi[i]->getPollFd().fd);
			_cgiSocketsFromCgi[i]->getPollFd().fd = 0;
			_cgiSocketsFromCgi.erase(_cgiSocketsFromCgi.begin() + i);
		}
		else if (type == Socket::cgiTo)
		{
			std::cerr << "[cgi To : fd " << _cgiSocketsToCgi[i]->getPollFd().fd << "] \n";
			close(_cgiSocketsToCgi[i]->getPollFd().fd);
			_cgiSocketsToCgi[i]->getPollFd().fd = 0;
			_cgiSocketsToCgi.erase(_cgiSocketsToCgi.begin() + i);
		}
		
		// need to clean the indexing that changed after the deletion
		_storeFdToPoll();
}

void		Hub::_closeAllConnections()
{	
	size_t 		nbOfSockets = 	_listenSockets.size() +
								_clientSockets.size() +
								_cgiSocketsFromCgi.size() +
								_cgiSocketsToCgi.size();
	
	// condition to be sure we don't try to close the connection
	// when there has been a problem (fail to bind for exemple)
	if (_nfds == nbOfSockets)
		for (size_t index = 0; index < nbOfSockets; index++)
			_closeConnection(0, _arr[0]->getType());
}


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Hub::Hub(std::string configFile) : 
		_nfds(),
		_clientSockets(),
		_listenSockets(),
		_cgiSocketsFromCgi(),
		_cgiSocketsToCgi(),
		_arr()
{
	Configuration::getInstance().parseConfigPath(configFile);
	memset(_fds, 0, sizeof(_fds));
}

Hub::~Hub()
{
	_closeAllConnections();
}