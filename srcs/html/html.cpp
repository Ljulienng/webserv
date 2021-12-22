#include "html.hpp"

namespace html {


std::string     buildErrorHtmlPage(std::string content)
{
    std::string     errorPage;

    errorPage += "<!DOCTYPE html>";
    errorPage += "<html>";
    errorPage += "<head>";
    errorPage += "<meta charset=\"utf-8\" />";
    errorPage += "</head>";
    errorPage += "<body>";
    errorPage += "<p>";
    errorPage += "ERROR ";
    errorPage += content;
    errorPage += "</p>";
    errorPage += "</body>";
    errorPage += "</html>";

    return errorPage;
}


}; // namespace html