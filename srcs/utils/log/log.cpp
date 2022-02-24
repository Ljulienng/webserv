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

    std::string     getTime()
    {
        time_t      rawTime;
        struct tm * timeinfo;
        char        currentTime[50];

        time(&rawTime);
        timeinfo = localtime(&rawTime);
        strftime(currentTime, 50, "%T", timeinfo);

        return currentTime;
    }

    std::string     getType(int type)
    {
        switch (type)
        {
            case Socket::server : return "server";
            case Socket::client : return "client";
            case Socket::cgiFrom : return "cgi from";
            case Socket::cgiTo : return "cgi to";
            default: return "unknow type";
        }
    }

    void			logEvent(std::string msg, int fd, int type)
    {
        std::cerr << "[ " << std::setw(8) << getTime() << " ] ";
        std::cerr << "[ " << std::left << std::setw(8) << getType(type) << " : ";
        std::cerr << std::left << std::setw(3)  << fd << " ] ";
        msg.find("closed") == std::string::npos ? std::cerr << ORG : std::cerr << BLU;
        std::cerr << msg  << RESET << std::endl;
    }

}