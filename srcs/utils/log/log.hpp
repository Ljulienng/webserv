#ifndef LOG_HPP
# define LOG_HPP

#include "webserv.hpp"

/* define not member functions -> toolbox */
namespace log {

    std::string     getTimestamp();
	void		    logEvent(std::string msg, int fd);

}

#endif