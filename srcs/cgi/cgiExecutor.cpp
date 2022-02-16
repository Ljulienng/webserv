#include "cgiExecutor.hpp"

void	CgiExecutor::initHeaders()
{
	char *buf = NULL;
	buf = getcwd(buf, 0);
	std::string cwd = static_cast<std::string>(buf) + "/";
	std::string newPath = _configMatch.root + _request->getUri().getPath();

	if (_request->getHeader("auth-scheme") != "")
		_env["AUTH_TYPE"] = _request->getHeader("Authorization");
	if (!_request->getBody().empty())
	{
		_env["CONTENT_LENGTH"] = _request->getHeader("Content-Length");
		_env["CONTENT_TYPE"] = _request->getHeader("Content-Type");
	}
	_env["GATEWAY_INFERFACE"] = "CGI/1.1";
	// _env["PATH_INFO"] = newPath; // version JU initiale
	_env["PATH_INFO"] = _request->getUri().getPath(); //cwd + _configMatch.pathTranslated; // new version test
	// _env["PATH_TRANSLATED"] = newPath; // version JU initiale
	_env["PATH_TRANSLATED"] = _configMatch.pathTranslated; // add to match changes of root in location block for example
	_env["QUERY_STRING"] = _request->getUri().getQuery();
	_env["REDIRECT_STATUS"] = "200";
	_env["REFERER"] = _request->getHeader("Referer");
	_env["REMOTE_ADDR"] = _request->getUri().getHost();
	_env["REMOTE_HOST"] = _request->getHeader("Hostname");
	_env["REMOTE_PORT"] = _request->getUri().getPort();
	_env["REMOTE_IDENT"] = _request->getHeader("Authorization");
	_env["REMOTE_USER"] = _request->getHeader("Authorization");
	_env["REQUEST_METHOD"] = _request->getMethod();
	_env["REQUEST_URI"] = _env.find("PATH_INFO")->second; // ajout
	_env["SCRIPT_FILENAME"] = /*"/app/wordpress/index.php"; */_env.find("PATH_TRANSLATED")->second; // ajout doit etre = PATH_INFO
	// _env["SCRIPT_NAME"] = newPath; // version JU initiale
	_env["SCRIPT_NAME"] = _request->getUri().getPath();
	_env["SERVER_NAME"] = _client->getServerName();
	_env["SERVER_PORT"] = _request->getUri().getPort();
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_SOFTWARE"] = "Webserver/1.0";

	_cgiPath = _configMatch.server.getCgi().second;
	_body = _request->getBody();

	/*
	* Store the argument variables in an array for excve usage
	*/
	_argArray = new char *[3];

	_argArray[0] = new char[_cgiPath.size() + 1];
	strcpy(_argArray[0], _cgiPath.c_str());
	_argArray[1] = new char[newPath.size() + 1];
	strcpy(_argArray[1], newPath.c_str());
	_argArray[2] = NULL;

	/*
	* Store the environnement variables in an array for excve usage
	*/

	size_t			 	i = 0;
	std::string			str;
	_envArray = new char *[_env.size() + 1];
	
	for	(std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++)
	{
		str = it->first + "=" + it->second; std::cerr << str << "\n";
		_envArray[i] = new char[str.size() + 1];
		strcpy(_envArray[i], str.c_str());
		i++;
	}
	_envArray[i] = NULL;
	// for (i = 0; _argArray[i]; i++)
	// 	std::cerr << _argArray[i] << std::endl;
}

static void		redirOut(int pipe[2])
{
	close(pipe[0]);
	dup2(pipe[1], STDOUT_FILENO);
	close(pipe[1]);
}
static void		redirIn(int pipe[2])
{
	close(pipe[1]);
	dup2(pipe[0], STDIN_FILENO);
	close(pipe[0]);
}

static void		createPipe(int pipeOut[2], int pipeIn[2])
{
	pipe(pipeOut);
	if (pipe(pipeIn) < 0)
	{
		close(pipeOut[0]);
		close(pipeOut[1]);
	}
}

void		CgiExecutor::execCgi()
{
	int pipeOut[2];
	int pipeIn[2];
	pid_t pid;

	createPipe(pipeOut, pipeIn);
	pid = fork();
	if (pid < 0)
	{
		close(pipeOut[0]);
		close(pipeOut[1]);
		close(pipeIn[0]);
		close(pipeIn[1]);
	}
	else if (pid == 0)
	{
		redirOut(pipeOut);
		redirIn(pipeIn);
		// chdir("/home/user42/Documents/Projects/12-Webserv/ourwebserv_");
		if ((execve(_cgiPath.c_str(), _argArray, _envArray)) == -1)
			throw (std::string("Can't execute the script")); // Error 500 to assign
		exit(errno);
	}
	else if (pid > 0)
	{
		// set non blocking connections
		fcntl(pipeOut[0], F_SETFL, O_NONBLOCK);
		_cgiSocketFromCgi = new CgiSocketFromCgi(pipeOut, _client, *_request);

		fcntl(pipeIn[1], F_SETFL, O_NONBLOCK);
		_cgiSocketToCgi = new CgiSocketToCgi(pipeIn, *_request);
	}
	clean();
}

/*
*	Closing all file, file descriptor and freeing the allocated array
*/	
void		CgiExecutor::clean()
{
	for (size_t i = 0; _envArray[i]; i++)
		delete [] _envArray[i];
	delete [] _envArray;

	for (size_t i = 0; _argArray[i]; i++)
		delete [] _argArray[i];
	delete [] _argArray;
}

std::string			CgiExecutor::getBody()
{ return std::string(_newBody.begin(), _newBody.end()); }

CgiSocketFromCgi*	CgiExecutor::getCgiSocketFromCgi()
{ return _cgiSocketFromCgi; }

CgiSocketToCgi*	CgiExecutor::getCgiSocketToCgi()
{ return _cgiSocketToCgi; }

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
CgiExecutor::CgiExecutor(Request request, ClientSocket* client, t_configMatch& configMatch) :
		_request(new Request(request)),
		_client(client),
		_cgiSocketFromCgi(),
		_cgiSocketToCgi(),
		_configMatch(configMatch)
			// to be completed if new attributes
{
	initHeaders();
}

CgiExecutor::~CgiExecutor()
{
	delete _request;
}
