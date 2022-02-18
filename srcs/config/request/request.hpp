#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "webserv.hpp"
#include "httpStatus.hpp"
#include "uri.hpp"
#include "str.hpp"

# define GET 1
# define POST 2
# define DELETE 3

class Request
{
private:
	// const std::string 	&_rawRequest;

	std::string									_method;
	std::string									_path;
	Uri											_uri;
	std::string									_version;
	std::map<std::string, std::string>			_headers;
	std::vector<std::pair<std::string, float> >	_acceptedLang; 
	std::string									_body;
	HttpStatus                        			_httpStatus;
	int											_ret;

public: 
	Request();
	Request(const std::string &request);
	Request(const Request &obj);
	~Request();

	Request			&operator=(const Request &obj);
	/* PARSING */
	void			initHeaders();
	int				verifArg();
	int				parseFirstLine(std::string line);
	std::string		nextLine(const std::string &request, size_t &i);
	void			storeKeyValue(const std::string &line);
	int				parseHeader(const std::string &request, size_t &i);
	void			setAcceptedLanguages();
	void			parseChunkedBody(const std::string &request);
	void			parsebody(const std::string &request);
	void			initUri();
	int 			parse(const std::string &request);
	int				verifBuffer(const std::string &buffer);

	/* GETTERS */

	// const std::string	&getRawRequest();
	std::string							&getMethod();
	std::string							&getPath();
	std::string							&getVersion();
	std::map<std::string, std::string>	&getHeaders();
	std::string                         getHeader(std::string key);
	std::string							&getBody();
	Uri									&getUri();
	HttpStatus							&getHttpStatus();
	int									getRet();

	/* DEBUG */
	void								debug();

};

int				checkRequest(std::string &buffer);


#endif