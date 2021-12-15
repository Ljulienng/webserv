#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include "webserv.hpp"
#include "server.hpp"
#include "str.hpp"
#include "file.hpp"
#include "clientSocket.hpp"

class ClientSocket;

/*
** informations from the configuration file : cgi, bodyMaxSize, a list of server(s) ...
*/
class Configuration
{
	public :

		typedef std::string::iterator	str_ite;


    private :

		std::string							_configFile;
		std::pair<std::string, std::string>	_cgi;
		size_t								_maxBodySize;
		std::map<int, std::string>			_errorPages;
		std::vector<Server>					_servers;
		std::vector<ClientSocket>			_clients;

		void 		_parseConfigPath(); 
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

		void		parse();
		Server 		&findServer(std::string serverName);
		void		debug();

		// SETTERS
		void		setConfigDatas(std::map<std::string, std::string> mapConfig);
		void		setCgi(std::string cgi);
		void		setMaxBodySize(std::string maxBodySize);

		// GETTERS
		std::pair<std::string, std::string>		&getCgi();
		size_t									&getMaxBodySize();
		std::map<int, std::string>				&getErrorPages();
		std::vector<Server>						&getServers();
		std::vector<ClientSocket>				&getClients();
		int										getTopServer();
};

#endif