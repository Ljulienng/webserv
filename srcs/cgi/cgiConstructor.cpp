#include "cgiConstructor.hpp"

/*
* Initialize environnement variables required for cgi using request informations
*/
void	cgiConstructor::initHeaders(Request &request,  t_configMatch &configMatch)
{
	if (request.getHeader("auth-scheme") != "")
		_env["AUTH_TYPE"] = request.getHeader("Authorization");
	_env["CONTENT_LENGTH"] = request.getHeader("Content-Length");
	_env["CONTENT_TYPE"] = request.getHeader("Content-Type");
	_env["GATEWAY_INFERFACE"] = "CGI/1.1";
	_env["PATH_INFO"] = request.getPath();
	_env["PATH_TRANSLATED"] = request.getPath();;
	_env["QUERY_STRING"] = request.getUri().getQuery();
	_env["REMOTE_ADDR"] = request.getUri().getPort();
	_env["REMOTE_HOST"] = request.getHeader("Hostname");
	_env["REMOTE_IDENT"] = request.getHeader("Authorization");
	_env["REMOTE_USER"] = request.getHeader("Authorization");
	_env["REQUEST_METHOD"] = request.getMethod();
	_env["SCRIPT_NAME"] = request.getPath();
	_env["SERVER_NAME"] = request.getHeader("Hostname");
	_env["SERVER_PORT"] = request.getUri().getPort();
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_SOFTWARE"] = "Webserver/1.0";
	_env["REDIRECT_STATUS"] = "200";

	_cgiPath = configMatch.server.getCgi().second;
	std::cout << _cgiPath << std::endl;
	_body = request.getBody();
	/*
	* Store the argument variables in an array for excve usage
	*/

	_argArray = new char *[3];

	// _argArray[0] = new char[_cgiPath.size() + 1];
	// strcpy(_argArray[0], _cgiPath.c_str());
	// _argArray[1] = new char[request.getPath().size() + 1];
	// strcpy(_argArray[1], request.getPath().c_str());
	_argArray[0] = NULL;
	_argArray[1] = NULL;
	_argArray[2] = NULL;

	/*
	* Store the environnement variables in an array for excve usage
	*/

	size_t			 	i = 0;
	std::string			str;
	_envArray = new char *[_env.size() + 1];
	
	for	(std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++)
	{
		str = it->first + "=" + it->second;
		_envArray[i] = new char[str.size() + 1];
		strcpy(_envArray[i], str.c_str());
		i++;
	}
	_envArray[i] = NULL;
	for (i = 0; _argArray[i]; i++)
			std::cout << _argArray[i] << std::endl;
	for (i = 0; _envArray[i]; i++)
			std::cout << _envArray[i] << std::endl;
}

std::vector<unsigned char>		cgiConstructor::execCgi()
{
	pid_t	pid;
	long	tmpFd[2];
	FILE	*fdFile[2]; // Temporary Files used to store cgi response
	long	fd[2];

	tmpFd[WRITE] = dup(STDIN_FILENO);
	tmpFd[READ] = dup(STDOUT_FILENO);

	/*
	* tmpfile opens a temporary binary file guaranteed to be different from any other existing files
	* fileno is used to map a stream pointer to a file descriptor
	*/
	fdFile[WRITE] = tmpfile();
	fdFile[READ] = tmpfile();
	fd[WRITE] = fileno(fdFile[WRITE]);
	fd[READ] = fileno(fdFile[READ]);
	write(fd[WRITE], _body.c_str(), _body.size()); // Assigning the body to be modified by the cgi

	if ((pid = fork()) == -1)
		throw (std::string("Fork crashed")); // Error 500 to assign
	if (pid == 0)
	{
		// char * const *null = NULL;

		lseek(fd[WRITE], 0, SEEK_SET); // Change the file offset to the beginning of the file
		dup2(fd[WRITE], STDIN_FILENO);
		dup2(fd[READ], STDOUT_FILENO);
		// execve(_cgiPath.c_str(), null, _envArray);
		if ((execve(_cgiPath.c_str(), _argArray, _envArray)) == -1)
			throw (std::string("Can't execute the script")); // Error 500 to assign
	// std::cout << "went here after clean" <<  std::endl;
	}
	else
		parse(fd);

	clean(tmpFd, fdFile, fd);
	if (pid == 0)
		exit(0);
	return (_newBody);
}

/*
*	Reading the cgi outputs and parsing the results into an unsigned char vector
*/	

void						cgiConstructor::parse(long fd[2])
{
	char	buffer[1024];
	int		ret = 1;

	waitpid(-1, NULL, 0);
	while (ret)
	{
		memset(buffer, 0, 1024);
		ret = read(fd[READ], buffer, 1023);
		_tmp += buffer;
	}

	for (size_t i = 0; _tmp.size(); i++)
		_newBody.push_back((unsigned char)_tmp[i]);
}

/*
*	Closing all file, file descriptor and freeing the allocated array
*/	
void						cgiConstructor::clean(long tmpFd[2], FILE *fdFile[2], long fd[2])
{
	dup2(tmpFd[WRITE], STDIN_FILENO);
	dup2(tmpFd[READ], STDOUT_FILENO);
	fclose(fdFile[WRITE]);
	fclose(fdFile[READ]);
	close(fd[WRITE]);
	close(fd[READ]);
	close(tmpFd[WRITE]);
	close(tmpFd[READ]);

	for (size_t i = 0; _envArray[i]; i++)
		delete [] _envArray[i];
	delete [] _envArray;

	for (size_t i = 0; _argArray[i]; i++)
		delete [] _argArray[i];
	delete [] _argArray;
}

/* GETTERS */

std::map<std::string, std::string>	cgiConstructor::getEnv()
{ return (_env); }

std::vector<unsigned char>			cgiConstructor::getBody()
{ return (_newBody); }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

cgiConstructor::cgiConstructor()
{}

cgiConstructor::cgiConstructor(Request &request, t_configMatch &t_configMatch)
{
	initHeaders(request, t_configMatch);
}

cgiConstructor::cgiConstructor(const cgiConstructor &obj) :
_env(obj._env), _envArray(obj._envArray)
{}

cgiConstructor::~cgiConstructor()
{}

cgiConstructor	&cgiConstructor::operator=(const cgiConstructor &obj)
{
	_env = obj._env;
	_envArray = obj._envArray;
	_body = obj._body;
	_newBody = obj._newBody;
	_cgiPath = obj._cgiPath;

	return (*this);
}