#ifndef URI_HPP
# define URI_HPP

#include "webserv.hpp"

class Uri
{
private:
	struct	Authority
	{
		std::string _userInfo;
		std::string	_host;
		std::string _port;
	};

	std::string		_scheme;
	Authority		_authority;
	std::string		_path;
	std::string		_query;
	std::string		_fragment;
	Uri();

public: 
	Uri(const std::string &url);
	Uri(const Uri &uri);
	~Uri();

	Uri			&operator=(const Uri &obj);
	/* PARSING */
	void urlParser(const std::string &url);

	/* GETTERS */
	std::string			&getScheme();
	std::string			&getPath();
	std::string			&getUserInfo();
	std::string			&getHost();
	std::string         &getPort();
	std::string			&getQuery();
	std::string			&getFragment();

	/* DEBUG */
	void								debug();

};


#endif