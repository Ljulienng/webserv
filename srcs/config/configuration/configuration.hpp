#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include "webserv.hpp"
#include "server.hpp"
#include "str.hpp"
#include "file.hpp"
#include "clientSocket.hpp"
#include "listeningSocket.hpp"
#include "singleton.hpp"

class ClientSocket;
/*
** informations from the configuration file : cgi, bodyMaxSize, a list of server(s) ...
*/
class Configuration : public Singleton<Configuration>
{
	public :

		typedef std::string::iterator	str_ite;


    private :

		std::string							_configFile;
		std::map<int, std::string>			_errorPages;
		std::vector<Server>					_servers;
		std::vector<ClientSocket>			_clients;

		void		_cleanSpaces(std::string &buf);
		size_t		_parseServer(str_ite it, str_ite ite);
		size_t		_parseErrorPages(str_ite it, str_ite ite);
		size_t		_parseLocation(str_ite it, str_ite ite, Server &server);
		size_t		_parseNextPair(str_ite it, str_ite ite, std::map<std::string, std::string> &m);
		bool		_isBloc(str_ite it, str_ite ite, std::string blocName);	
		bool		_checkConfigScope(str_ite it, str_ite ite);


    public : 
	     
		Configuration();
		Configuration(std::string configFile);
		Configuration(const Configuration &src);
		~Configuration();	
		Configuration &operator=(const Configuration &src);

		void 		parseConfigPath(std::string configFile);
		void		parse();
		Server 		&findServer(std::string serverName);
		void		debug();

		// GETTERS
		std::map<int, std::string>			&getErrorPages();
		std::vector<Server>					&getServers();
		std::vector<ClientSocket>			&getClients();
};

#endif