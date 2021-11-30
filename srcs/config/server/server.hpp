#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"
#include "location.hpp"
#include "str.hpp"

/*
** information from the configuration file : server_name, ip, port, maxBodySize, a list of location(s) ...
*/
class Server
{
    private :

		std::string					_name;
		std::string					_ip;
		unsigned short				_port;
		size_t						_maxBodySize; 
		std::vector<Location>		_locations;
		struct sockaddr_in 			_sockaddr;
		int							_sockfd;

		void 	_initAddr();
		void 	_setSocketOpt();


    public :

        Server();
		Server(const Server &src);
		~Server();

		Server &operator=(const Server &src);

		void	addLocation(Location location);
		int 	start();
		

		// SETTERS
		void	setServerDatas(std::map<std::string, std::string> mapServer);
		void	setName(std::string name);
		void	setIp(std::string ip);
		void	setPort(std::string port);
		void	setMaxBodySize(std::string maxBodySize);

		// GETTERS
		std::string				&getName();
		std::string				&getIp();
		unsigned short			&getPort();
		std::vector<Location>	&getLocations();
		size_t					&getMaxBodySize();
		int						&getSockfd();
};

#endif