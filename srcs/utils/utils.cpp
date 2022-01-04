#include "utils.hpp"

namespace utils {

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

    std::string     myItoa(size_t number)
    {
        std::stringstream   stream;

        stream << number;
        return stream.str();
    }

}