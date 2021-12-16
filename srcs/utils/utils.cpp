#include "utils.hpp"

namespace utils {

    std::string    getTimestamp()
    {
        time_t      rawTime;
        struct tm * timeinfo;
        char        currentTime[50];

        time(&rawTime);
        timeinfo = gmtime(&rawTime);
        strftime(currentTime, 50, "%Y-%m-%d  %T GMT", timeinfo);

        return currentTime;
    }

    std::string     myItoa(size_t number)
    {
        std::stringstream   stream;

        stream << number;
        return stream.str();
    }

}