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
}

void	Hub::process()
{
	_storeFdToPoll(); 
	int pollRet = poll(_fds, _nfds, 1000); // call poll and wait for an event
	if (pollRet < 0) // poll failed or SIGINT received [poll is a blocking function and SIGINT will unblock it]
	{	
		_closeAllConnections();
		return ;
	}
	static size_t cgiCount[MAX_CGI_RUNNING] = {0};
	for (size_t i = 0; i < _nfds; i++) 
	{
		// only way found to detect end of cgi POLLIN to send response to the client
		if (_fds[i].revents == 0 && _arr[i]->getType() == Socket::cgiFrom && cgiCount[i] > 0)
		{
			_prepareCgiResponse(_arr[i]->_index);
			//close both connections at the same time
			_closeConnection(_arr[i]->_index, _arr[i]->getType());		
			_closeConnection(_arr[i]->_index, Socket::cgiTo);
			cgiCount[i] = 0;
		}
		else if (_fds[i].revents == 0)
			continue;
		else if ((_fds[i].revents & POLLIN) == POLLIN)
		{
			if (_arr[i]->getType() == Socket::server)
				_acceptIncomingConnections(i);
			else if (_arr[i]->getType() == Socket::client)
			{
				if (!_receiveRequest(i)) // client not disconnected
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
			_closeConnection(0, Socket::client); // disconnect the first client
		int acceptRet = accept(_arr[i]->getPollFd().fd, NULL, NULL);

		if (acceptRet == -1) // no connection is present
			break ;

		_addClientSocket(acceptRet, _arr[i]);

		// add the new incoming connection to the pollfd structure
		log::logEvent("New incoming connection", acceptRet);

		// loop back up and accept another incoming connection
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
	std::vector<char>	buffer(MAX_BUF_LEN);
	ClientSocket* 		client = _clientSockets[_arr[i]->_index];
	std::vector<Server> servers = Configuration::getInstance().getServers();
	bool				close = false;
	// // struct timeval timeout;      
    // // timeout.tv_sec = 0;
    // // timeout.tv_usec = 1;


	// // if (setsockopt (client->getPollFd().fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
    // //             sizeof timeout) < 0)
    //     // std::cout << "setsockopt failed";
	// fcntl(client->getPollFd().fd, F_SETFL, O_NONBLOCK);

	bytes = recv(client->getPollFd().fd, &buffer[0], MAX_BUF_LEN, 0);
	client->_bytes += bytes;
	// std::cout << "bytes = " << bytes << " ret = " << ret << std::endl;
	
	if (bytes < 0)
	{
		_closeAllConnections();
		exit(EXIT_FAILURE);
	}
	else if (bytes > 0)
	{
		client->getBuffer().append(buffer.begin(), lastChar(buffer));
		std::cout << "buffer = "<< client->getBuffer();
		if ((checkRet = checkRequest(client->getBuffer())) == GOOD)
		{
			client->addRequest();
			if (client->getRequests().back().getBody().size() > servers[indexServer(*client)].getMaxBodySize())
				client->getRequests().back().getHttpStatus().setStatus(413);
			std::cout << "error code = " << client->getRequests().back().getHttpStatus().getCode() << std::endl;
			log::logEvent("Received a new request", client->getFd());
		}
	}
	else //bytes = 0;
	{
		_closeConnection(_arr[i]->_index, _arr[i]->getType()); // disconnect the client
		close = true;
	}
	return close;
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
	std::list<Request>		&requests = client->getRequests();

	while (requests.empty() == false)
	{
		std::list<Request>::iterator it = client->getRequests().begin();
		for (; it != client->getRequests().end(); it++)
		{
			t_configMatch 	configMatch = getConfigMatch(*it, client->getServerName());
			Response 		response;
			
			if (isErrorStatus(it->getHttpStatus().getCode()))
			{
				response = errorResponse(response, configMatch, it->getHttpStatus().getCode());
				client->getResponses().push_back(response);
			}
			else if (_needCgi(*it, configMatch))
			{	
				CgiExecutor cgi(*it, client, configMatch); // copie the request to have an empty pool of request and leave the loop
				cgi.execCgi(); // execute cgi and create 2 cgi sockets (in and out)
				_cgiSocketsFromCgi.push_back(cgi.getCgiSocketFromCgi());
				_cgiSocketsToCgi.push_back(cgi.getCgiSocketToCgi());
			}
			else
			{
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
	Response 		response;
	t_configMatch 	configMatch;
	
	configMatch = getConfigMatch(_cgiSocketsFromCgi[i]->getRequest(), _cgiSocketsFromCgi[i]->getClient()->getServerName());
	response = cgiResponse(_cgiSocketsFromCgi[i]->getBuffer(), response, configMatch);
	
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
			delete _listenSockets[i];
			_listenSockets.erase(_listenSockets.begin() + i);
		}
		else if (type == Socket::client)
		{
			std::cerr << "[client : fd " << _clientSockets[i]->getPollFd().fd << "] \n";
			close(_clientSockets[i]->getPollFd().fd);
			delete _clientSockets[i];
			_clientSockets.erase(_clientSockets.begin() + i);
		}
		else if (type == Socket::cgiFrom)
		{
			std::cerr << "[cgi From : fd " << _cgiSocketsFromCgi[i]->getPollFd().fd << "] \n";
			close(_cgiSocketsFromCgi[i]->getPollFd().fd);
			_cgiSocketsFromCgi[i]->getPollFd().fd = 0;
			close(_cgiSocketsFromCgi[i]->getFdUseless());
			delete _cgiSocketsFromCgi[i];
			_cgiSocketsFromCgi.erase(_cgiSocketsFromCgi.begin() + i);
		}
		else if (type == Socket::cgiTo)
		{
			std::cerr << "[cgi To : fd " << _cgiSocketsToCgi[i]->getPollFd().fd << "] \n";
			close(_cgiSocketsToCgi[i]->getPollFd().fd);
			_cgiSocketsToCgi[i]->getPollFd().fd = 0;
			close(_cgiSocketsToCgi[i]->getFdUseless());
			delete _cgiSocketsToCgi[i];
			_cgiSocketsToCgi.erase(_cgiSocketsToCgi.begin() + i);
		}
			
		// need to clean the indexing that changed after the deletion
		_storeFdToPoll();
}

void		Hub::_closeAllConnections()
{	
	size_t tmp = _nfds;
	for (size_t i = 0; i < tmp; i++)
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