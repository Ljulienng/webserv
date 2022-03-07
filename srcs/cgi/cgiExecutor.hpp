#ifndef CGIEXECUTOR_HPP
# define CGIEXECUTOR_HPP

#include "socket.hpp"
#include "request.hpp"
#include "response.hpp"
#include "configMatch.hpp"
#include "cgiSocketFromCgi.hpp"
#include "cgiSocketToCgi.hpp"

typedef struct s_configMatch t_configMatch;

class CgiExecutor
{
    protected :

		std::map<std::string, std::string>	_env;
		char								**_argArray;
		char								**_envArray;
		std::string							_body;
		std::string							_cgiPath;
        Request*                            _request;
        ClientSocket*                       _client;
        CgiSocketFromCgi*                   _cgiSocketFromCgi;
        CgiSocketToCgi*                     _cgiSocketToCgi;
        t_configMatch&                      _configMatch;
    

    public :
        CgiExecutor(Request request, ClientSocket* client, t_configMatch& configMatch);
        virtual ~CgiExecutor();	

        void                    initHeaders();
        void                    execCgi();
        void	                clean();

        CgiSocketFromCgi*       getCgiSocketFromCgi();
        CgiSocketToCgi*         getCgiSocketToCgi();
};

#endif