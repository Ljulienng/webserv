#ifndef CGICONSTRUCTOR_HPP
# define CGICONSTRUCTOR_HPP

#include "webserv.hpp"
#include "request.hpp"
#include "responseConstructor.hpp"
#include "configMatch.hpp"

typedef struct s_configMatch t_configMatch;

class cgiConstructor
{
	private:
		std::map<std::string, std::string>	_env;
		char								**_argArray;
		char								**_envArray;
		std::string							_body;
		std::string							_tmp;
		std::vector<unsigned char>			_newBody;
		std::string							_cgiPath;

		cgiConstructor();

	public:
		cgiConstructor(const cgiConstructor &obj);
		cgiConstructor(Request &request, t_configMatch &t_configMatch);

		void								initHeaders(Request &request, t_configMatch &configMatch);
		std::vector<unsigned char>			execCgi();
		std::vector<unsigned char>			myexecCgi();
		void								loloexecCgi();


		~cgiConstructor();
		void								parse(long fd[2]);
		void								clean(long tmpFd[2], FILE *fdFile[2], long fd[2]);

		/* GETTERS */
		std::map<std::string, std::string>	getEnv();
		std::vector<unsigned char>			getBody();

		cgiConstructor	&operator=(const cgiConstructor	&obj);
};

#endif