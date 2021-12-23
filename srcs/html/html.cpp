#include "html.hpp"

namespace html {


std::string     buildAutoIndexPage(std::string path)
{
    File                                dir(path);
    std::list<std::string>              files = dir.buildFilesList();
    std::list<std::string>::iterator    it = files.begin();
    std::string                         autoIndexPage;

    autoIndexPage += "<!DOCTYPE html>";
    autoIndexPage += "<html>";
    autoIndexPage += "<head>";
    autoIndexPage += "<meta charset=\"utf-8\" />";
    autoIndexPage += "</head>";
    autoIndexPage += "<body>";
    autoIndexPage += "<h1> Index of </h1";
    for ( ; it != files.end(); it++)
    {
        std::string newUrl = path + "/" + *it;
        File file(newUrl);
        if (file.isRegularFile())
            autoIndexPage += "  <a href=\"./" + *it +  "\">"  + *it + "</a> <br/>\n";
        else if (file.isDirectory())
            autoIndexPage += "  <a href=\"./" + *it +  "/\">"  + *it + "/ </a> <br/>\n";
    }
    autoIndexPage += "</body>";
    autoIndexPage += "</html>";

    return autoIndexPage;
}

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