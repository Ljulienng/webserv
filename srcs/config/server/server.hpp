#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm> // count
#include "../location/location.hpp"
#include "../../utils/str/str.hpp"

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

    public :
        Server();
		Server(const Server &src);
		~Server();

		Server &operator=(const Server &src);

		void			addLocation(Location location);
		
		// SETTERS
		void	setServerDatas(std::map<std::string, std::string> mapServer);
		void	setName(std::string name);
		void	setIp(std::string ip);
		void	setPort(std::string port);
		void	setMaxBodySize(std::string maxBodySize);

		// GETTERS
		std::string					&getName();
		std::string					&getIp();
		unsigned short				&getPort();
		std::vector<Location>		&getLocations();
		size_t						&getMaxBodySize();
};

#endif