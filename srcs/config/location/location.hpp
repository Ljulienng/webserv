#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "webserv.hpp"
#include "str.hpp"

class Location
{
    private :
		std::vector<std::string>	_acceptedMethod; // GET POST DELETE
		std::string					_path;
		std::string					_root;
		std::string					_defaultFile;
		bool						_autoindex;
		size_t						_maxBodySize; 
		std::pair<int, std::string>	_redirections;

    public :
        Location();
		Location(const Location &src);
		~Location();

		Location &operator=(const Location &src);

		std::vector<std::string>	&getAcceptedMethod();
		std::string					&getPath();
		std::string					&getRoot();
		bool						&getAutoindex();
		size_t						&getMaxBodySize();

		// SETTERS
		void	setLocationsDatas(std::map<std::string, std::string>  mapLocation);
		void	setPath(std::string path);
		void	setRoot(std::string root);
		void	setMethods(std::string methods);
		void	setDefaultFile(std::string default_file);
		void	setAutoindex(std::string autoIndex);
		void	setMaxBodySize(std::string maxBodySize);
		void	setRedirections(std::string redirection);	
};

#endif