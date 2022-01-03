#include "hub.hpp"

void	Hub::start()
{
	Configuration::getInstance().parse();
	// Configuration::getInstance().debug();
	_startSockets();
}

/*
** start socket for each server of the configuration file
*/
void		Hub::_startSockets()
{
	std::vector<Server> &servers = Configuration::getInstance().getServers();

	for (size_t i = 0; i < servers.size(); i++)
	{
		servers[i].start();
		_fds[_nfds].fd = servers[i].getSocket().getFd();
		_fds[_nfds].events = POLLIN;
		_nfds++;
	}
}

/*
** main loop
*/
void	Hub::process()
{
	int pollRet = pollRet = poll(_fds, _nfds, -1); // call poll and wait an infinite time
	
	if (pollRet < 0) // poll failed or SIGINT received [poll is a blocking function and SIGINT will unblock it]
	{
		_closeAllConnections();
		return ;
	}

	// one or more fd are readable. Need to determine which ones they are
	for (size_t i = 0; i < _nfds; i++)
	{
		// loop to find the fd that returned POLLIN and determine whether it's the listening or the active connection
		if (_fds[i].revents == 0)
			continue;
		
		// if revent is POLLIN, there is data waiting to be read
		if (_fds[i].revents == POLLIN)
		{
			// if the current fd is one of our servers, we connect a new client (listening descriptor is readable)
			if (i < Configuration::getInstance().getServers().size()) // fd stored after "nb of servers" are clients fd and not servers
			{
				_acceptIncomingConnections(i);
			}
			// it's not a listening socket (server), therefore an existing connection must be readable (client)
			// it's a client so we need to receive the request (recv), parse it and prepare response
			else
			{
				_receiveRequest(i);
				_prepareResponse(i);
			}
		}
		else if (_fds[i].revents == POLLOUT)
		{
			_sendResponse(i);
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
void		Hub::_acceptIncomingConnections(size_t index)
{
	std::vector<Server> 		&servers = Configuration::getInstance().getServers();
	std::vector<ClientSocket> 	&clients = Configuration::getInstance().getClients();

	while (42)
	{
		if (_nfds == MAX_CONNECTIONS)
			_closeConnection(0, CLIENT); // disconnect the first client
		int acceptRet = accept(_fds[index].fd, NULL, NULL);

		if (acceptRet == -1) // no connection is present
			break ;

		ClientSocket client;
		client.setServerName(servers[index].getName());
		client.setFd(acceptRet);
		client.setPort(servers[index].getPort());
		clients.push_back(client);

		// add the new incoming connection to the pollfd structure
		_output("New incoming connection", acceptRet);
		_fds[_nfds].fd = acceptRet;
		_fds[_nfds].events = POLLIN;
		_nfds++;
		// loop back up and accept another incoming connection
	}
}

/*
** receive and parse the request
*/
void		Hub::_receiveRequest(size_t index)
{
	std::vector<Server> 		&servers = Configuration::getInstance().getServers();
	std::vector<ClientSocket> 	&clients = Configuration::getInstance().getClients();
	size_t						clientIndex = index - servers.size();
	int 						bytes = 0;
	std::vector<char>			buffer(MAX_BUF_LEN);

	bytes = recv(_fds[index].fd, &buffer[0], MAX_BUF_LEN, 0);
	if (bytes < 0)
		throw std::string("Error: can't receive client request");
	else if (bytes > 0)
	{
		clients[clientIndex].getBuffer().append(buffer.begin(), buffer.end());
		while (bytes == MAX_BUF_LEN)
		{
			buffer.clear();
			bytes = recv(_fds[index].fd, &buffer[0], MAX_BUF_LEN, 0);
			if (bytes < 0)
				throw std::string("Error: can't receive client request");
			else
				clients[clientIndex].getBuffer().append(buffer.begin(), buffer.end());
		}
		clients[clientIndex].addRequest(); // move here not to create the request if bytes = 0
	}
	else //bytes = 0;
		_closeConnection(clientIndex, CLIENT); // disconnect the client

	_output("Received a new request", clients[clientIndex].getFd());
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
void		Hub::_prepareResponse(size_t index)
{
	std::vector<Server> 		&servers = Configuration::getInstance().getServers();
	std::vector<ClientSocket> 	&clients = Configuration::getInstance().getClients();

	size_t					clientIndex = index - servers.size();
	std::queue<Request>		&requests = clients[clientIndex].getRequests();

	while (requests.empty() == false)
	{
		Request 	req = requests.front();
		//req.debug();
		
		// Response 	response(req, clients[clientIndex].getServerName()); // check errors and build response (thanks to req elements)
		Response 	response; //(req, clients[clientIndex].getServerName()); // check errors and build response (thanks to req elements)
		response = constructResponse(req, clients[clientIndex].getServerName());

		requests.pop();

		clients[clientIndex].getResponses().push(response);
	}

	// the socket is now ready to write in addition to reading because we have added a response
	if (clients[clientIndex].getResponses().empty() == false)
		_fds[index].events = POLLIN | POLLOUT;
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
void 		Hub::_sendResponse(size_t index)
{
	std::vector<Server> 		&servers = Configuration::getInstance().getServers();
	std::vector<ClientSocket> 	&clients = Configuration::getInstance().getClients();
	size_t						clientIndex = index - servers.size();
	std::queue<Response>		&responses = clients[clientIndex].getResponses();
	std::string					buffer = clients[clientIndex].getBuffer();

	// we process the responses one by one and append them to the client buffer
	// then delete the response until the queue is empty
	while (responses.empty() == false)
	{
		Response response = responses.front(); // process in FIFO order, we process the oldest element first

		// then have to check header and status of the response
		// ...

		std::string		message = response.getMessage(); // put into a string the response
		buffer.insert(buffer.end(), message.begin(), message.end()); // append it to the client buffer
		_output("Response sent", clients[clientIndex].getFd());
		responses.pop(); // delete the response
	}

	if (buffer.empty() == false)
	{
		// we can use send() ( without flag parameter, send is equivalent to write() )
		// so write into the _fds[i].fd the content of buffer
		// std::cout << "Response = \n" << &buffer[0] << "\n";
		int bytes = write(_fds[index].fd, &buffer[0], buffer.size());

		if (bytes <= 0)
		{
			_closeAllConnections();
			exit(EXIT_FAILURE);
		}
		buffer.erase(buffer.begin(), buffer.begin() + bytes); // clear the buffer

		_fds[index].events = POLLIN; // finished to write so we are now waiting for reading
	}
}

void		Hub::_closeConnection(size_t index, int type)
{
	std::vector<Server> 		&servers = Configuration::getInstance().getServers();
	std::vector<ClientSocket> 	&clients = Configuration::getInstance().getClients();

	if (type == SERVER)
	{
		_output("Connection closed - server", servers[index].getSocket().getFd());
		close(servers[index].getSocket().getFd());
		servers.erase(servers.begin() + index);
		for (size_t i = index; i < _nfds; i++)
			_fds[i] = _fds[i + 1];
	}
	else 
	{
		_output("Connection closed - client", clients[index].getFd());
		close(clients[index].getFd());
		clients.erase(clients.begin() + index);
		for (size_t i = index + servers.size(); i < _nfds; i++)
			_fds[i] = _fds[i + 1];	
	}
	_nfds--;
}

void		Hub::_closeAllConnections()
{
	size_t 		nbOfServers = Configuration::getInstance().getServers().size();
	size_t 		nbOfClients = Configuration::getInstance().getClients().size();

	for (size_t index = 0; index < nbOfServers; index++)
		_closeConnection(0, SERVER);
	for (size_t index = 0; index < nbOfClients; index++)
		_closeConnection(0, CLIENT);
}

void			Hub::_output(std::string msg, int fd)
{
	std::cout << " [ fd " << fd << " ]  " << ORG << msg  << RESET << std::endl;
}


/* SETTERS */
void		Hub::setNfds(int nfds)
{ _nfds = nfds; }

struct pollfd *		Hub::getFds()
{ return _fds; }

size_t		Hub::getNfds()
{ return _nfds; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Hub::Hub() : _nfds() 
			// to be completed if new attributes
{
	memset(_fds, 0, sizeof(_fds));
}

Hub::Hub(std::string configFile) : _nfds() 
{
	Configuration::getInstance().parseConfigPath(configFile);
	memset(_fds, 0, sizeof(_fds));
}

Hub::Hub(const Hub &src)
{
	*this = src;
}

// find another way to close connection
// if fail to bind, try to close connection and segfault
Hub::~Hub()
{
	// _closeAllConnections();
}

Hub &Hub::operator=(const Hub &src)
{
	if (&src != this)
	{
		_nfds = src._nfds;
		// to be completed if new attributes
	}
	return (*this);
}