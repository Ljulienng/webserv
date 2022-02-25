#ifndef LOG_HPP
# define LOG_HPP

#include "webserv.hpp"
#include "socket.hpp"

/* define not member functions -> toolbox */
namespace log {

    std::string     getTimestamp();
    std::string     getTime();
    std::string     getType(int type);
	void		    logEvent(std::string msg, int fd, int type);

}

#endif