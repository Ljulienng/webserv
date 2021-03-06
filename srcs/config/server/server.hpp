#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"
#include "location.hpp"
#include "socket.hpp"
#include "str.hpp"
#include "file.hpp"

class Server
{
    private :

		std::string							_name;
		std::string							_ip;
		unsigned short						_port;
		std::string							_root;
		// std::string							_index;
		size_t								_maxBodySize; 
		std::string							_uploadPath;
		std::vector<Location>				_locations;
		std::pair<std::string, std::string>	_cgi;
		
		Location		*_findExactLocation(std::string uriRequest);
		

    public :

        Server();
		Server(const Server &src);
		~Server();
		Server &operator=(const Server &src);

		void		debug(size_t index);
		void		addLocation(Location location);
		Location	&findLocation(std::string uriRequest);

		// SETTERS
		void		setServerDatas(std::map<std::string, std::string> mapServer);
		void		setName(std::string name);
		void		setIp(std::string ip);
		void		setPort(std::string port);
		void		setRoot(std::string root);
		void		setMaxBodySize(std::string maxBodySize);
		void		setUploadPath(std::string uploadPath);
		void		setCgi(std::string cgi);

		// GETTERS
		std::string								&getName();
		std::string								&getIp();
		unsigned short							&getPort();
		std::string								&getRoot();
		size_t									&getMaxBodySize();
		std::string								&getUploadPath();
		std::vector<Location>					&getLocations();
		std::pair<std::string, std::string>		&getCgi();
};

#endif