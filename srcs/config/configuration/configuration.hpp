#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include "webserv.hpp"
#include "server.hpp"
#include "str.hpp"
#include "file.hpp"
#include "clientSocket.hpp"

/*
** informations from the configuration file : cgi, bodyMaxSize, a list of server(s) ...
*/
class Configuration
{
    private :
		std::string							_configFile;
		std::pair<std::string, std::string>	_cgi;
		size_t								_maxBodySize;
		std::map<int, std::string>			_errorPages;
		std::map<size_t/*std::string*/, Server>		_servers; // test to have an index instead of the server name
		std::map<size_t, ClientSocket>		_clients; // test index+client connectes (size_t = index pour matcher dans les _fds apres les serveurs)
		struct pollfd						_fds[MAX_CONNECTIONS];
		size_t								_nfds;

		void 		_parseConfigPath(); 
		void		_cleanSpaces(std::string &buf);
		size_t		_parseServer(std::string::iterator it, std::string::iterator ite);
		size_t		_parseErrorPages(std::string::iterator it, std::string::iterator ite);
		size_t		_parseLocation(std::string::iterator it, std::string::iterator ite, Server &server);
		size_t		_parseNextPair(std::string::iterator it, std::string::iterator ite, std::map<std::string, std::string> &m);
		bool		_isBloc(std::string::iterator it, std::string::iterator ite, std::string blocName);	

    public :       
		Configuration();
		Configuration(std::string configFile);
		Configuration(const Configuration &src);
		~Configuration();	

		Configuration &operator=(const Configuration &src);

		void		parse();
		void		startSockets();
		void		debug();

		// SETTERS
		void		setConfigDatas(std::map<std::string, std::string> mapConfig);
		void		setCgi(std::string cgi);
		void		setMaxBodySize(std::string maxBodySize);
		void		setNfds(int nfds);

		// GETTERS
		std::pair<std::string, std::string>		&getCgi();
		size_t									&getMaxBodySize();
		std::map<int, std::string>				&getErrorPages();
		std::map<size_t/*std::string*/, Server>	&getServers();
		std::map<size_t, ClientSocket>			&getClients();
		struct pollfd *							getFds();
		size_t									getNfds();
		int										getTopServer();
};

#endif