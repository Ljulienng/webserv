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
			// if the current fd is one of our servers, we connect a new client (listening descriptor is readable)
			if (i < _config.getServers().size()) // fd stored after "nb of servers" are clients fd and not servers
			{
				_acceptIncomingConnections(i);
			}
			// it's not a listening socket (server), therefore an existing connection must be readable (client)
			// it's a client so we need to receive the request (recv), parse it and prepare response
			else
			{
				// RECEIVE THE REQUEST (recv)

				// int 				bytes = 0;
				// std::vector<char>	buffer(MAX_BUF_LEN);


				// while (bytes == MAX_BUF_LEN) // Loop to get all the data into the buffer MAX_BUF_LEN at a time
				// {
				// 	bytes = recv(_config.getFds()[i].fd, &buffer[0], MAX_BUF_LEN, 0);
				// 	if (bytes == -1)
				// 		throw std::string("Error: can't receive client request");
				// 	else
				// 		_config.getClients()[i].getBuffer().append(buffer.begin(), buffer.end());
				// }
				// // std::cout << _config.getClients()[j].getBuffer() << std::endl;
				// for (int i = 0; i < bytes; i++)
				// 	std::cout << buffer[i];
				// buffer.clear();
				// bytes = 0;	
				// std::cout << "Went in else" << std::endl;
				// // RECEIVE THE REQUEST (recv)
				int 				bytes = 0;
				std::vector<char>	buffer(MAX_BUF_LEN);

				bytes = recv(_config.getFds()[i].fd, &buffer[0], MAX_BUF_LEN, 0);
				if (bytes < 0)
					throw std::string("Error: can't receive client request");
				else if (bytes > 0)
				{
					_config.getClients()[i].getBuffer().append(buffer.begin(), buffer.end());
					while (bytes == MAX_BUF_LEN)
					{
						buffer.clear();
						bytes = recv(_config.getFds()[i].fd, &buffer[0], MAX_BUF_LEN, 0);
						if (bytes < 0)
							throw std::string("Error: can't receive client request");
						else
							_config.getClients()[i].getBuffer().append(buffer.begin(), buffer.end());
					}
				}
				else
					bytes = 0;
				_config.getClients()[i].addRequest();
				// PARSE THE REQUEST

				// PREPARE THE RESPONSE
				_prepareResponse(i);
			}
		}
		else if (_config.getFds()[i].revents == POLLOUT)
		{
			std::cout << i <<  " POLLOUT\n";
			_sendResponse(i);
		}
		else
		{
			std::cout << i <<  " ERROR QUIT PROGRAM\n";
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
		int acceptRet = accept(_config.getFds()[index].fd, NULL, NULL);

		if (acceptRet == -1) // no connection is present
			break ;

		ClientSocket client;
		client.setFd(acceptRet);
		client.setPort(_config.getServers().find(index)->second.getPort());
		// insert new client at index 'i + nb of servers' to find it easily
		_config.getClients().insert(std::pair<size_t, ClientSocket>(index + _config.getServers().size(), client));
		
		// add the new incoming connection to the pollfd structure
		std::cout << "New incoming connection - fd : " << acceptRet << "\n";
		std::cout << "nfds = " << _config.getNfds()<< "\n";
		_config.getFds()[_config.getNfds()].fd = acceptRet;
		_config.getFds()[_config.getNfds()].events = POLLIN;
		_config.setNfds(_config.getNfds() + 1);
		// loop back up and accept another incoming connection
	}
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
	std::queue<Request>		requests = _config.getClients().find(index)->second.getRequests();
	
	while (requests.empty() == false)
	{
		Request 	req = requests.front();
		Response 	resp;

		// check errors and build response
		// build resp thanks to req elements
		// ...
		resp.setContent("TEST"); 

		requests.pop();
		if (_config.getClients().find(index) != _config.getClients().end())
		{
			_config.getClients().find(index)->second.getResponses().push(resp);
			// the socket is now ready to write in addition to reading because we have added a response
			if (_config.getClients().find(index)->second.getResponses().empty() == false)
				_config.getFds()[index].events = POLLIN | POLLOUT;
		}
	}
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
	// SEND RESPONSE [ get responses queue of the client, send them and delete them ]
	std::map<size_t, ClientSocket>::iterator	client = _config.getClients().find(index);
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

		std::string		message = response.getMessage(); // put into a string the response
		buffer.insert(buffer.end(), message.begin(), message.end()); // append it to the client buffer

		responses.pop(); // delete the response
	}

	if (buffer.empty() == false)
	{
		// we can use send() ( without flag parameter, send is equivalent to write() )
		// so write into the _fds[i].fd the content of buffer
		int bytes = write(_config.getFds()[index].fd, &buffer[0], buffer.size());

		if (bytes <= 0)
			exit(EXIT_FAILURE); // disconnect each sockets (server and clients) and exit
		buffer.erase(buffer.begin(), buffer.begin() + bytes); // clear the buffer

		// finished to write so we are now waiting for reading
		_config.getFds()[index].events = POLLIN;
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