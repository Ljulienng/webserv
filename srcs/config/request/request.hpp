#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "webserv.hpp"

class Request
{
private:
	const std::string 	&_rawRequest;

	std::string							_method;
	std::string							_path;
	std::string							_version;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
	int									_port;
	int									_ret;

	Request();
	Request(const Request &obj);
public: 
	Request(const std::string &request);
	~Request();

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

	const std::string	&getRawRequest();
	std::string			&getMethod();
	std::string			&getPath();
	std::string			&getVersion();
	std::string			&getBody();
	int					getPort();

};


#endif