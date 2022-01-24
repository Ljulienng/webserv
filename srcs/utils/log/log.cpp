#include "log.hpp"

namespace log { 

    // rfc 2616 : 3.3.1
    std::string    getTimestamp()
    {
        time_t      rawTime;
        struct tm * timeinfo;
        char        currentTime[50];

        time(&rawTime);
        timeinfo = gmtime(&rawTime);
        strftime(currentTime, 50, "%a, %d %b %Y %T GMT", timeinfo);

        return currentTime;
    }

    void			logEvent(std::string msg, int fd)
    {
        std::cout << "[ fd " << fd << " ]  " << ORG << msg  << RESET << std::endl;
    }

}