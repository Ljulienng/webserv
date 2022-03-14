#include "hub.hpp"

/*
** parse configuration file
** start socket for each server of the configuration file
*/
void	Hub::start(std::string configFile)
{
	Configuration::getInstance().parseConfigPath(configFile);
	Configuration::getInstance().parse();
	
	std::vector<Server> &servers = Configuration::getInstance().getServers();
	for (size_t i = 0; i < servers.size(); i++)
		_addListeningSocket(servers[i]);
}

/*
** if start of listening Socket fails, need to delete it and close its fd
*/
void	Hub::clean()
{
	for (std::vector<ListeningSocket*>::iterator it = _listenSockets.begin(); it != _listenSockets.end(); it++)
	{
		close((*it)->getFd());
		delete *it;
	}
	_listenSockets.clear();
}

void	Hub::_addListeningSocket(Server& server)
{
	ListeningSocket* 	newListenSocket = new ListeningSocket(server.getName());
	struct pollfd 		newPollFd;

	newPollFd.fd = socket(AF_INET, SOCK_STREAM, 0);
	newPollFd.events = POLLIN;
	newListenSocket->setPollFd(newPollFd);
	if (newListenSocket->start(server.getIp(), server.getPort()))
	{
		_listenSockets.push_back(newListenSocket);
		throw (std::string("Can't start the listening socket"));
	}
	_listenSockets.push_back(newListenSocket);
	log::logEvent("Listen on " + server.getIp() + ":" + myItoa(server.getPort()), newListenSocket->getFd(), Socket::server);
	_storeFdToPoll();
}

void	Hub::_addClientSocket(int acceptRet, Socket* listenSocket)
{
	ClientSocket* 	newClientSocket = new ClientSocket();
	struct pollfd 	newPollFd;

	newClientSocket->setServerName(listenSocket->getServerName());
	newClientSocket->setFd(acceptRet);
	newClientSocket->setPort(listenSocket->getAddr().sin_port);
	newPollFd.fd = acceptRet;
	newPollFd.events = POLLIN;
	newClientSocket->setPollFd(newPollFd);
	_clientSockets.push_back(newClientSocket);
}

/*
** before calling poll()
** need to put each fd (sockets and files) in a single array (_fds[])
*/
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
	for (std::vector<ClientSocket*>::iterator it = _clientSockets.begin(); it != _clientSockets.end(); it++)
	{
		for (std::list<Response*>::iterator itR = (*it)->getResponses().begin(); itR != (*it)->getResponses().end(); itR++)
		{
			if ((*itR)->getStateFile() != Response::NONE)
			{
				_fds[_nfds] = (*itR)->getPollFdFile();
				(*itR)->setIndexFile(_nfds);
				_nfds++;
			}		
		}
	}
}

void	Hub::_checkTimeout()
{
	size_t i = 0;
	for (std::vector<ClientSocket*>::iterator it = _clientSockets.begin(); it != _clientSockets.end(); it++)
	{
		if ((*it)->getTimeout())
		{
			std::cerr << "Timeout\n";
			_closeClientConnection(*it, i);
			break ;
		}
		i++;
	}
}
/*
** wait for an event on a fd
** query poll each second
*/
int		Hub::_waitPollEvent()
{
	int	pollRet = 0;

	_checkTimeout();
	_storeFdToPoll();
	pollRet = poll(_fds, _nfds, 1000);
	return pollRet;
}

/*
** if there is nothing more to read on a cgifrom fd
** prepare response and close both connections
*/
bool	Hub::_cgiClosed(size_t i, size_t *cgiCount)
{
	size_t cgiIndex = _arr[i]->_index;
	_prepareCgiResponse(cgiIndex);
	_closeConnection(cgiIndex, Socket::cgiFrom);
	_closeConnection(cgiIndex, Socket::cgiTo);
	cgiCount[i] = 0;
	return true;
}

void	Hub::process()
{
	if (_waitPollEvent() < 0) // poll failed or SIGINT received [poll is a blocking function and SIGINT will unblock it]
	{
		_closeAllConnections();
		return ;
	}

	static size_t cgiCount[MAX_CGI_RUNNING] = {0};
	for (size_t i = 0; i < _nfds - g_fileArr.size(); i++) 
	{	
		// only way found to detect end of cgi POLLIN to send response to the client
		if (_fds[i].revents == 0 && _arr[i]->getType() == Socket::cgiFrom && cgiCount[i] > 0)
		{
			_cgiClosed(i, cgiCount);
			break ;
		}
		else if (_fds[i].revents == 0)
			continue;
		else if ((_fds[i].revents & POLLIN) == POLLIN)
		{
			if (_arr[i]->getType() == Socket::server)
				_acceptIncomingConnections(i);
			else if (_arr[i]->getType() == Socket::client)
			{
				if (!_receiveRequest(i)) // client disconnected
					break ;
				_prepareResponse(i);				
			}
			else if (_arr[i]->getType() == Socket::cgiFrom)
			{
				cgiCount[i]++;
				if (_cgiSocketsFromCgi[_arr[i]->_index]->readFromCgi() == ERROR)
				{
					_closeClientConnection(_cgiSocketsFromCgi[_arr[i]->_index]->getClient(), _cgiSocketsFromCgi[_arr[i]->_index]->getClient()->_index);
					break ;
				}
			}
		}
		else if ((_fds[i].revents & POLLOUT) == POLLOUT)
		{
			if (_arr[i]->getType() == Socket::client)
			{
				if (!_sendResponse(i))
					break ;
			}
			else if (_arr[i]->getType() == Socket::cgiTo)
			{
				if (_cgiSocketsToCgi[_arr[i]->_index]->writeToCgi() == ERROR)
				{
					_closeClientConnection(_cgiSocketsFromCgi[_arr[i]->_index]->getClient(), _cgiSocketsFromCgi[_arr[i]->_index]->getClient()->_index);
					break ;
				}
			}
		}
		else if ((_fds[i].revents & POLLERR) == POLLERR || (_fds[i].revents & POLLHUP) == POLLHUP)
		{
			_closeConnection(_arr[i]->_index, _arr[i]->getType());
			break ;
		}
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
bool		Hub::_acceptIncomingConnections(size_t i)
{
	while (1)
	{
		if (_clientSockets.size() == MAX_CONNECTIONS)
		{	
			std::cerr << "Exceed max number of fd - the server resets connections" << std::endl;
			_closeAllConnections();
			return false;
			//return _closeClientConnection(_clientSockets[0], 0);
		}

		int acceptRet = accept(_arr[i]->getFd(), NULL, NULL);
		if (acceptRet == -1) // no connection is present
			break ;

		_addClientSocket(acceptRet, _arr[i]);

		// add the new incoming connection to the pollfd structure
		log::logEvent("New incoming connection", acceptRet, Socket::client);
	}
	return true;
}

static size_t	indexServer(ClientSocket client)
{
	size_t i = 0;
	std::vector<Server> servers = Configuration::getInstance().getServers();
	for ( ; i < servers.size(); i++)
		if (servers[i].getName() == client.getServerName())
			return i;
	return i;
}

/*
** receive and parse the request
*/
bool		Hub::_receiveRequest(size_t i)
{
	int 				bytes = 0, checkRet = 0;
	std::vector<char>	buffer(BUF_SIZE);
	ClientSocket* 		client = _clientSockets[_arr[i]->_index];
	std::vector<Server> servers = Configuration::getInstance().getServers();
	
	bytes = recv(client->getFd(), &buffer[0], BUF_SIZE, 0);
	if (bytes <= 0)
		return _closeClientConnection(client, _arr[i]->_index);
	else
	{
		client->setTimeout();
		client->getBuffer().append(buffer.begin(), buffer.begin() + bytes);
		if ((checkRet = checkRequest(client->getBuffer())) == GOOD && bytes < BUF_SIZE)
		{
			client->addRequest();
			if (client->getRequests().back().getBody().size() > servers[indexServer(*client)].getMaxBodySize())
				client->getRequests().back().getHttpStatus().setStatus(413);
			log::logEvent("Received a new request", client->getFd(), Socket::client);
		}
	}

	return true;
}

static bool _needCgi(Request request, t_configMatch configMatch)
{
	File        path(configMatch.pathTranslated);

	if (configMatch.server.getCgi().first == "." + getExtension(configMatch.pathTranslated)
			// && request.getPath().find(configMatch.server.getCgi().first) != std::string::npos
			&& configMatch.pathTranslated.find(configMatch.server.getCgi().first) != std::string::npos
			&& (request.getMethod() == "GET" || request.getMethod() == "POST"))
		return true;

	return false;
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
	std::list<Request>&		requests = client->getRequests();
	
	while (requests.empty() == false)
	{
		std::list<Request>::iterator it = client->getRequests().begin();
		
		for (; it != client->getRequests().end(); it++)
		{
			t_configMatch 	configMatch = getConfigMatch(*it, client->getServerName());
			
			if (isErrorStatus(it->getHttpStatus().getCode()))
			{
				Response* 		response = new Response();
				errorResponse(response, configMatch, it->getHttpStatus().getCode());
				client->addResponse(response);
				// client is now ready to write in addition to reading because we have added a response
				client->getPollFd().events = POLLIN | POLLOUT;
			}
			else if (_needCgi(*it, configMatch))
			{
				// execute cgi and create 2 cgi sockets (in and out)
				CgiExecutor cgi(*it, client, configMatch); // copy the request to have an empty pool of request and leave the loop
				_cgiSocketsFromCgi.push_back(cgi.getCgiSocketFromCgi());
				_cgiSocketsToCgi.push_back(cgi.getCgiSocketToCgi());
			}
			else
			{
				Response* 		response = new Response();
				constructResponse(response, *it, configMatch);
				client->addResponse(response);
				// client is now ready to write in addition to reading because we have added a response
				client->getPollFd().events = POLLIN | POLLOUT;
			}
			requests.erase(it++);
		}
	}

	// the socket is now ready to write in addition to reading because we have added a response
	// if (client->getResponses().empty() == false)
	// 	client->getPollFd().events = POLLIN | POLLOUT;
}

void		Hub::_prepareCgiResponse(size_t i)
{
	Response*		response = new Response();
	t_configMatch 	configMatch;

	configMatch = getConfigMatch(_cgiSocketsFromCgi[i]->getRequest(), _cgiSocketsFromCgi[i]->getClient()->getServerName());
	cgiResponse(response, _cgiSocketsFromCgi[i]->getBuffer(), configMatch);
	
	_cgiSocketsFromCgi[i]->getClient()->addResponse(response);
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
bool 		Hub::_sendResponse(size_t i)
{
	ClientSocket* 			client = _clientSockets[_arr[i]->_index];
	std::list<Response*>&	responses = client->getResponses();
	std::string				buffer = client->getBuffer();
	bool					endOfResponse = false, endToReadFile = false, endToWriteFile = false;

	client->setTimeout();
	// we process the responses one by one and append them to the client buffer
	std::list<Response*>::iterator it = responses.begin();
	if (it != responses.end())
	// for (std::list<Response*>::iterator it = responses.begin(); it != responses.end(); it++)
	{
		endOfResponse = false;
		endToReadFile = false;
		endToWriteFile = false;

		if ((*it)->getIndexFile() == -1)						// not a file (directory for example)
			endOfResponse = true;			
		else if (_fds[(*it)->getIndexFile()].revents & POLLIN) 	// GET
		{														// data to read in a file	
			if((*it)->readFile(&endOfResponse, &endToReadFile) == ERROR) 
				return _closeClientConnection(client, _arr[i]->_index);
		}
		else if (_fds[(*it)->getIndexFile()].revents & POLLOUT) // POST
		{														// data to write in a file
			if ((*it)->writeFile(&endOfResponse, &endToWriteFile) == ERROR)	
				return _closeClientConnection(client, _arr[i]->_index);
		}
		else
		{
			endOfResponse = true;
			endToReadFile = true;
		}
		
		// we can add the response to the buffer
		if (endOfResponse)	
		{
			if (endToReadFile || endToWriteFile)
				(*it)->endToReadorWrite();
			_nfds--;
			std::string	message = (*it)->getMessage();
			buffer.insert(buffer.end(), message.begin(), message.end());
			delete (*it);
			// responses.erase(it++);
			responses.erase(it);
		}
	}

	// responses have been processed, we can send to the client
	if (responses.empty() && !buffer.empty())
	{
		// so write into the _fds[i].fd the content of buffer
		int bytes = write(client->getFd(), &buffer[0], buffer.size());
		if (bytes <= 0)
			return _closeClientConnection(client, _arr[i]->_index);
		log::logEvent("Response sent", client->getFd(), Socket::client);
		buffer.erase(buffer.begin(), buffer.begin() + bytes);
		
		if (buffer.empty()) // finished to write so we are now waiting for reading
			client->getPollFd().events = POLLIN;
	}

	return true;
}

/*
** before closing a client check if cgiSockets have been created for this client
** if so, close them first
*/
void 	Hub::_closeCgiConnections(ClientSocket* client)
{
	for (size_t i = 0; i < _cgiSocketsFromCgi.size(); i++)
	{
		if (_cgiSocketsFromCgi[i]->getClient() == client)
		{
			_closeConnection(i, Socket::cgiFrom);
			_closeConnection(i, Socket::cgiTo);
		}
	}
}

bool	Hub::_closeClientConnection(ClientSocket* client, size_t clientIndex)
{
	_closeCgiConnections(client);
	_closeConnection(clientIndex, Socket::client);
	return false;
}

void		Hub::_closeConnection(size_t i, int type)
{
	if (type == Socket::server)
	{
		log::logEvent("Connection closed", _listenSockets[i]->getFd(), Socket::server);
		delete _listenSockets[i];
		_listenSockets.erase(_listenSockets.begin() + i);
	}
	else if (type == Socket::client)
	{
		log::logEvent("Connection closed", _clientSockets[i]->getFd(), Socket::client);
		delete _clientSockets[i];
		_clientSockets.erase(_clientSockets.begin() + i);
	}
	else if (type == Socket::cgiFrom)
	{
		log::logEvent("Connection closed", _cgiSocketsFromCgi[i]->getFd(), Socket::cgiFrom);
		delete _cgiSocketsFromCgi[i];
		_cgiSocketsFromCgi.erase(_cgiSocketsFromCgi.begin() + i);
	}
	else if (type == Socket::cgiTo)
	{
		log::logEvent("Connection closed", _cgiSocketsToCgi[i]->getFd(), Socket::cgiTo);
		delete _cgiSocketsToCgi[i];
		_cgiSocketsToCgi.erase(_cgiSocketsToCgi.begin() + i);
	}

	// need to clean the indexing that changed after the deletion
	_storeFdToPoll();
}

void		Hub::_closeAllConnections()
{	
	
	size_t tmp = _nfds - g_fileArr.size();
	if (tmp)
	{
		for (size_t i = 0; i < tmp; i++)
			_closeConnection(0, _arr[0]->getType());
		for (size_t i = 0; i < g_fileArr.size(); i++)
			close(g_fileArr[i]->fd);
	}
}


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Hub::Hub() : 
		_nfds(),
		_clientSockets(),
		_listenSockets(),
		_cgiSocketsFromCgi(),
		_cgiSocketsToCgi(),
		_arr()
{
	memset(_fds, 0, sizeof(_fds));
}

Hub::~Hub()
{
	_closeAllConnections();
}