#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"
#include "str.hpp"

class Location
{
    private :
	
		std::vector<std::string>	_acceptedMethod;
		std::string					_path;
		std::string					_root;
		std::string					_index;
		bool						_autoindex;
		size_t						_maxBodySize; 
		std::pair<int, std::string>	_redirection;
		// need to set the type of action : cgi, redirection or nothing


    public :

        Location();
		Location(const Location &src);
		~Location();

		Location &operator=(const Location &src);

		void	debug(size_t index);

		// SETTERS
		void	setLocationsDatas(std::map<std::string, std::string>  mapLocation);
		void	setPath(std::string path);
		void	setRoot(std::string root);
		void	setMethods(std::string methods);
		void	setIndex(std::string index);
		void	setAutoindex(std::string autoIndex);
		void	setMaxBodySize(std::string maxBodySize);
		void	setRedirection(std::string redirection);	

		// GETTERS
		std::vector<std::string>	&getAcceptedMethod();
		std::string					&getPath();
		std::string					&getRoot();
		std::string					&getIndex();
		bool						&getAutoindex();
		size_t						&getMaxBodySize();
		std::pair<int, std::string>	&getRedirection();
};


// NON MEMBER FUNCTIONS
int isValidExpression(std::string expression, const char **validExpressions);

static const char* configExpression[] = {
	"error_pages",
	"server",
	0
};

static const char* serverExpression[] = {
	"server_name",
	"ip",
	"port",
	"root",
	"index",
	"max_body_size",
	"upload_path",
	"cgi",
	"location",
	0
};

static const char* locationExpression[] = {
	"path",
	"root",
	"accepted_methods",
	"index",
	"autoindex",
	"max_body_size",
	"redirection",
	0
};

#endif