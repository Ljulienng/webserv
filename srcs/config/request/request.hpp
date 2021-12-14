#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "webserv.hpp"
#include "str.hpp"


class Request
{
private:
	// const std::string 	&_rawRequest;

	std::string							_method;
	std::string							_path;
	std::string							_version;
	std::map<std::string, std::string>	_headers;
	std::map<std::string, float>		_acceptedLang; 
	std::string							_body;
	int									_ret;

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
	void			parsebody(const std::string &request);
	int 			parse(const std::string &request);

	/* GETTERS */

	// const std::string	&getRawRequest();
	std::string							&getMethod();
	std::string							&getPath();
	std::string							&getVersion();
	std::map<std::string, std::string>	&getHeaders();
	std::string							&getBody();

	/* DEBUG */
	void								debug();

};


#endif