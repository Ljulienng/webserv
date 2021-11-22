#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

class Server
{
private:
	struct sockaddr_in _sockaddr;
	int				_sockfd;

	Server();
public:
	Server(const Server &src);
	~Server();

	Server &operator=(const Server &src);
	int start();
	void fillAddr();
};


#endif