#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"
#include "location.hpp"
#include "str.hpp"
#include "socket.hpp"

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
		Socket						_socket;
		

    public :

        Server();
		Server(const Server &src);
		~Server();
		Server &operator=(const Server &src);

		void		debug(size_t index);
		void 		start();
		void		createSocket();
		void		closeSocket();
		void		bindSocket();
		void		setSocketOptions();
		void		setNonBlock();
		void		listenSocket();
		void		addLocation(Location location);
		// Location 	&findLocation(std::string uri);

		// SETTERS
		void		setServerDatas(std::map<std::string, std::string> mapServer);
		void		setName(std::string name);
		void		setIp(std::string ip);
		void		setPort(std::string port);
		void		setMaxBodySize(std::string maxBodySize);

		// GETTERS
		std::string				&getName();
		std::string				&getIp();
		unsigned short			&getPort();
		size_t					&getMaxBodySize();
		std::vector<Location>	&getLocations();
		Socket 					&getSocket();
};

#endif