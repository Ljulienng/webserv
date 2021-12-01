#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include "webserv.hpp"
#include "server.hpp"
#include "str.hpp"
#include "file.hpp"

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
		std::map<std::string, Server>		_servers;
		struct pollfd						_fds[MAX_CONNECTIONS];
		int									_nfds;

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
		std::map<std::string, Server>			&getServers();
		struct pollfd *							getFds();
		int										getNfds();
};

// NON MEMBER FUNCTIONS
bool isValidExpression(std::string expression, const char **validExpressions);


static const char* ConfigExpression[] = {
	"cgi",
	"max_body_size",
	"error_pages",
	"server",
	0
};

static const char* ServerExpression[] = {
	"server_name",
	"ip",
	"port",
	"max_body_size",
	"location",
	0
};

static const char* LocationExpression[] = {
	"path",
	"root",
	"accepted_methods",
	"redirection",
	"max_body_size",
	"default_file",
	"autoindex",
	0
};

#endif