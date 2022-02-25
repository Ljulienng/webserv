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

void	Hub::_addListeningSocket(Server& server)
{
	ListeningSocket* 	newListenSocket = new ListeningSocket(server.getName());
	struct pollfd 		newPollFd;

	newPollFd.fd = socket(AF_INET, SOCK_STREAM, 0);;
	newPollFd.events = POLLIN;
	newListenSocket->setPollFd(newPollFd);
	newListenSocket->start(server.getIp(), server.getPort());
	_listenSockets.push_back(newListenSocket);
	log::logEvent("Listen on " + server.getIp() + ":" + myItoa(server.getPort()), newListenSocket->getFd(), Socket::server);
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

int		Hub::_waitPollEvent()
{
	int	pollRet = 0;

	_storeFdToPoll();
	pollRet = poll(_fds, _nfds, 1000);
	
	return pollRet;
}

bool	Hub::_cgiClosed(size_t i, size_t *cgiCount)
{
	static size_t timer[MAX_CGI_RUNNING] = {0};

	timer[i]++;
	if (timer[i] > 1)
	{
		_prepareCgiResponse(_arr[i]->_index);
		_closeConnection(_arr[i]->_index, Socket::cgiFrom);
		_closeConnection(_arr[i]->_index, Socket::cgiTo);
		cgiCount[i] = 0;
		timer[i] = 0;
		return true;
	}
	return false;
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
			if (_cgiClosed(i, cgiCount))
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
				_cgiSocketsFromCgi[_arr[i]->_index]->readFromCgi();
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
				_cgiSocketsToCgi[_arr[i]->_index]->writeToCgi();
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
void		Hub::_acceptIncomingConnections(size_t i)
{
	while (1)
	{
		if (_clientSockets.size() == MAX_CONNECTIONS)
		{
			_closeConnection(0, Socket::client); // disconnect the first client
			return ;
		}
		int acceptRet = accept(_arr[i]->getFd(), NULL, NULL);

		if (acceptRet == -1) // no connection is present
			break ;

		_addClientSocket(acceptRet, _arr[i]);

		// add the new incoming connection to the pollfd structure
		log::logEvent("New incoming connection", acceptRet, Socket::client);
	}
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
	{
		_closeConnection(_arr[i]->_index, _arr[i]->getType()); // disconnect the client
		return false;
	}
	else
	{
		client->getBuffer().append(buffer.begin(), lastChar(buffer));
		if ((checkRet = checkRequest(client->getBuffer())) == GOOD && bytes < BUF_SIZE)
		// if (bytes < BUF_SIZE)
		{	std::cerr<<"bytes = " <<bytes<<"\n";std::cerr << "[_receiveRequest] buffer :\n" << std::string(client->getBuffer().begin(), client->getBuffer().begin() + 100) << "\n";
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

	if (configMatch.server.getCgi().first == ".php"
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
				client->getResponses().push_back(response);
			}
			else if (_needCgi(*it, configMatch))
			{
				// execute cgi and create 2 cgi sockets (in and out)
				CgiExecutor cgi(*it, *client, configMatch); // copy the request to have an empty pool of request and leave the loop
				_cgiSocketsFromCgi.push_back(cgi.getCgiSocketFromCgi());
				_cgiSocketsToCgi.push_back(cgi.getCgiSocketToCgi()); 
			}
			else
			{
				Response* 		response = new Response();
				constructResponse(response, *it, configMatch);
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
	Response*		response = new Response();
	t_configMatch 	configMatch;

	configMatch = getConfigMatch(_cgiSocketsFromCgi[i]->getRequest(), _cgiSocketsFromCgi[i]->getClient().getServerName());
	cgiResponse(response, _cgiSocketsFromCgi[i]->getBuffer(), configMatch);
	
	_cgiSocketsFromCgi[i]->getClient().getResponses().push_back(response);
	_cgiSocketsFromCgi[i]->getClient().getPollFd().events = POLLIN | POLLOUT;
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

	// we process the responses one by one and append them to the client buffer
	for (std::list<Response*>::iterator it = responses.begin(); it != responses.end(); it++)
	{
		if ((*it)->getHeader("Connection") == "close")
		{
			_closeConnection(_arr[i]->_index, _arr[i]->getType());
			return false;
		}

		if ((*it)->getIndexFile() == -1)						// not a file (directory for example)
			endOfResponse = true;			
		else if (_fds[(*it)->getIndexFile()].revents & POLLIN) 	// getRequest
			(*it)->readFile(&endOfResponse, &endToReadFile); 	// data to read in a file	
		else if (_fds[(*it)->getIndexFile()].revents & POLLOUT) // postRequest
			(*it)->writeFile(&endOfResponse, &endToWriteFile);	// data to write in a file
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
			responses.erase(it++);
		}
	}
	
	// responses have been processed, we can send to the client
	if (responses.empty() && !buffer.empty())
	{
		// so write into the _fds[i].fd the content of buffer
		int bytes = write(client->getFd(), &buffer[0], buffer.size());
		log::logEvent("Response sent", client->getFd(), Socket::client);
		if (bytes <= 0)
		{
			_closeAllConnections();
			exit(EXIT_FAILURE);
		}
		buffer.erase(buffer.begin(), buffer.begin() + bytes);
		if (buffer.empty()) // finished to write so we are now waiting for reading
			client->getPollFd().events = POLLIN;
	}
	return true;
}

void		Hub::_closeConnection(size_t i, int type)
{
		if (type == Socket::server)
		{
			log::logEvent("Connection closed", _listenSockets[i]->getFd(), Socket::server);
			close(_listenSockets[i]->getFd());
			delete _listenSockets[i];
			_listenSockets.erase(_listenSockets.begin() + i);
		}
		else if (type == Socket::client)
		{
			log::logEvent("Connection closed", _clientSockets[i]->getFd(), Socket::client);
			close(_clientSockets[i]->getFd());
			delete _clientSockets[i];
			_clientSockets.erase(_clientSockets.begin() + i);
		}
		else if (type == Socket::cgiFrom)
		{
			log::logEvent("Connection closed", _cgiSocketsFromCgi[i]->getFd(), Socket::cgiFrom);
			close(_cgiSocketsFromCgi[i]->getFd());
			_cgiSocketsFromCgi[i]->getFd() = 0;
			close(_cgiSocketsFromCgi[i]->getFdUseless());
			delete _cgiSocketsFromCgi[i];
			_cgiSocketsFromCgi.erase(_cgiSocketsFromCgi.begin() + i);
		}
		else if (type == Socket::cgiTo)
		{
			log::logEvent("Connection closed", _cgiSocketsToCgi[i]->getFd(), Socket::cgiTo);
			close(_cgiSocketsToCgi[i]->getFd());
			_cgiSocketsToCgi[i]->getFd() = 0;
			close(_cgiSocketsToCgi[i]->getFdUseless());
			delete _cgiSocketsToCgi[i];
			_cgiSocketsToCgi.erase(_cgiSocketsToCgi.begin() + i);
		}

		// need to clean the indexing that changed after the deletion
		_storeFdToPoll();
}

void		Hub::_closeAllConnections()
{	
	size_t tmp = _nfds - g_fileArr.size();
	tmp = _listenSockets.size() + _clientSockets.size() + _cgiSocketsFromCgi.size() + _cgiSocketsToCgi.size();
	for (size_t i = 0; i < tmp; i++)
		_closeConnection(0, _arr[0]->getType());
	for (size_t i = 0; i < g_fileArr.size(); i++)
		close(g_fileArr[i]->fd);
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