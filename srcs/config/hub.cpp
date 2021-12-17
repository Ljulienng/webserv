#include "hub.hpp"

void	Hub::start()
{
	_config.parse();
	_startSockets();
}

/*
** start socket for each server of the configuration file
*/
void		Hub::_startSockets()
{
	for (size_t i = 0; i < _config.getServers().size(); i++)
	{
		_config.getServers()[i].start();
		_fds[_nfds].fd = _config.getServers()[i].getSocket().getFd();
		_fds[_nfds].events = POLLIN;
		_nfds++;
	}
}

/*
** main loop
*/
void	Hub::process()
{
	int pollRet = 1;
		
	// call poll and wait an infinite time
	pollRet = poll(_fds, _nfds, -1);
	if (pollRet < 0) // poll() failed
	{
		// if (SIGINT)
		// 	return ;
		// else
		// {
		// 	_closeAllConnections();
		// 	exit(EXIT_FAILURE);
		// }
		return ;
	}
	// one or more fd are readable. Need to determine which ones they are
	for (size_t i = 0; i < _nfds; i++)
	{
		// loop through to find the fd that returned POLLIN and determine whether it's the listening or the active connection
		if (_fds[i].revents == 0)
			continue;
		
		// if revent is POLLIN, there is data waiting to be read
		if (_fds[i].revents == POLLIN)
		{
			// std::cout <<  " POLLIN\n";
			// if the current fd is one of our servers, we connect a new client (listening descriptor is readable)
			if (i < _config.getServers().size()) // fd stored after "nb of servers" are clients fd and not servers
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
			// std::cout <<  " POLLOUT\n";
			_sendResponse(i);
		}
		else
		{
			// std::cout << " ERROR QUIT PROGRAM\n";
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
	while (42)
	{
		if (_nfds == MAX_CONNECTIONS)
			_closeConnection(0, CLIENT); // disconnect the first client
		int acceptRet = accept(_fds[index].fd, NULL, NULL);

		if (acceptRet == -1) // no connection is present
			break ;

		ClientSocket client;
		std::cout << "new fd (accept) = " << acceptRet << "\n";
		client.setServerName(_config.getServers()[index].getName());
		client.setFd(acceptRet);
		client.setPort(_config.getServers()[index].getPort());
		std::cout << "new fd client to add (accept) = " << client.getFd() << "\n";
		_config.getClients().push_back(client);

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
	size_t				clientIndex = index - _config.getServers().size();
	int 				bytes = 0;
	std::vector<char>	buffer(MAX_BUF_LEN);

	bytes = recv(_fds[index].fd, &buffer[0], MAX_BUF_LEN, 0);
	if (bytes < 0)
		throw std::string("Error: can't receive client request");
	else if (bytes > 0)
	{
		_config.getClients()[clientIndex].getBuffer().append(buffer.begin(), buffer.end());
		while (bytes == MAX_BUF_LEN)
		{
			buffer.clear();
			bytes = recv(_fds[index].fd, &buffer[0], MAX_BUF_LEN, 0);
			if (bytes < 0)
				throw std::string("Error: can't receive client request");
			else
				_config.getClients()[clientIndex].getBuffer().append(buffer.begin(), buffer.end());
		}
	}
	else //bytes = 0;
		_closeConnection(clientIndex, CLIENT); // disconnect the client

	// PARSE THE REQUEST
	_config.getClients()[clientIndex].addRequest();
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
	size_t					clientIndex = index - _config.getServers().size();
	std::queue<Request>		&requests = _config.getClients()[clientIndex].getRequests();

	while (requests.empty() == false)
	{
		Request 	req = requests.front();
		//req.debug();
		Response 	resp(req, _config, _config.getClients()[clientIndex].getServerName()); // check errors and build response (thanks to req elements)

		requests.pop();

		_config.getClients()[clientIndex].getResponses().push(resp);
	}

	// the socket is now ready to write in addition to reading because we have added a response
	if (_config.getClients()[clientIndex].getResponses().empty() == false)
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
	size_t					clientIndex = index - _config.getServers().size();
	std::queue<Response>	&responses = _config.getClients()[clientIndex].getResponses();
	std::string				buffer = _config.getClients()[clientIndex].getBuffer();

	// we process the responses one by one and append them to the client buffer
	// then delete the response until the queue is empty
	while (responses.empty() == false)
	{
		// process in FIFO order, we process the oldest element first
		Response response = responses.front();

		// then have to check header and status of the response
		// ...

		std::string		message = response.getMessage(); // put into a string the response
		buffer.insert(buffer.end(), message.begin(), message.end()); // append it to the client buffer
		_output("Response sent", _config.getClients()[clientIndex].getFd());
		responses.pop(); // delete the response
	}

	if (buffer.empty() == false)
	{
		// we can use send() ( without flag parameter, send is equivalent to write() )
		// so write into the _fds[i].fd the content of buffer
		std::cout << "fd to send response = " << _fds[index].fd << "  at index = " << index << "\n";
		int bytes = write(_fds[index].fd, &buffer[0], buffer.size());

		if (bytes <= 0)
			exit(EXIT_FAILURE); // disconnect each sockets (server and clients) and exit
		buffer.erase(buffer.begin(), buffer.begin() + bytes); // clear the buffer

		// finished to write so we are now waiting for reading
		_fds[index].events = POLLIN;
	}
	std::cout << "index of client fd to close = " << clientIndex << "  fd = " << _config.getClients()[clientIndex].getFd() << "\n"; 
	// _closeConnection(clientIndex, CLIENT); // _disconnect client
}

void		Hub::_closeConnection(size_t index, int type)
{
	if (type == SERVER)
	{
		std::cout << "Need to disconnect server here" << index << "\n";
		_output("Connection closed", _config.getServers()[index].getSocket().getFd());
		close(_config.getServers()[index].getSocket().getFd());
		_config.getServers().erase(_config.getServers().begin() + index);
		for (size_t i = index; i < _nfds; i++)
			_fds[i] = _fds[i + 1];
	}
	else 
	{
		std::cout << "Need to disconnect client here" << index << "\n";
		_output("Connection closed", _config.getClients()[index].getFd());
		close(_config.getClients()[index].getFd());
		_config.getClients().erase(_config.getClients().begin() + index);
		for (size_t i = index + _config.getServers().size(); i < _nfds; i++)
			_fds[i] = _fds[i + 1];	
	}
	_nfds--;
}

void		Hub::_closeAllConnections()
{
	std::cout
	for (size_t index = 0; index < _config.getServers().size(); index++)
		_closeConnection(index, SERVER);
	for (size_t index = 0; index < _config.getClients().size(); index++)
	{
		_closeConnection(index, CLIENT);
		_output("Connection closed", _config.getClients()[index].getFd());
		close(_config.getClients()[index].getFd());
		_config.getClients().erase(_config.getClients().begin() + index);
		for (size_t i = 0; i < _nfds; i++)
			_fds[i] = _fds[i + 1];
		_nfds--;
	}
}

void			Hub::_output(std::string msg, int fd)
{
	std::cout << " [ " << fd << " ]  " << ORG << msg  << RESET << std::endl;
}


/* SETTERS */
void		Hub::setNfds(int nfds)
{ _nfds = nfds; }


/* GETTERS */
Configuration	&Hub::getConfig()
{ return _config; }

struct pollfd *		Hub::getFds()
{ return _fds; }

size_t		Hub::getNfds()
{ return _nfds; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
Hub::Hub() : _config(), _nfds() 
			// to be completed if new attributes
{
	memset(_fds, 0, sizeof(_fds));
}

Hub::Hub(std::string configFile) : _config(configFile), _nfds() 
{
	memset(_fds, 0, sizeof(_fds));
}

Hub::Hub(const Hub &src)
{
	*this = src;
}

Hub::~Hub()
{
	// _closeAllConnections();
}

Hub &Hub::operator=(const Hub &src)
{
	if (&src != this)
	{
		_config = src._config;
		_nfds = src._nfds;
		// to be completed if new attributes
	}
	return (*this);
}