#include "html.hpp"

namespace html {


std::string     buildAutoIndexPage(std::string path)
{
    File                                dir(path);
    std::list<std::string>              files = dir.buildFilesList();
    std::list<std::string>::iterator    it = files.begin();
    std::string                         autoIndexPage;

    autoIndexPage =
    "<html>\n"
    "   <head>\n"
    "       <meta charset=\"UTF-8\" />\n"
    "   </head>\n"
    "   <body>\n"
    "   <h1> Index of </h1>\n";
    for ( ; it != files.end(); it++)
    {
        std::string newUrl = path + "/" + *it;
        File file(newUrl);
        if (file.isRegularFile())
            autoIndexPage += "  <a href=\"./" + *it +  "\">"  + *it + "</a> <br/>\n";
        else if (file.isDirectory())
            autoIndexPage += "  <a href=\"./" + *it +  "/\">"  + *it + "/ </a> <br/>\n";
    }
    autoIndexPage +=
    "   </body>\n"
    "</html>\n";

    return autoIndexPage;
}

std::string     buildRedirectionPage(std::pair<int, std::string> redirection)
{
    std::string     redirectionPage;
    HttpStatus      status(redirection.first);

    redirectionPage = 
        "<html>\n"
        "   <head>\n"
        "       <meta charset=\"UTF-8\" />\n"
        "       <title> Redirection " + utils::myItoa(redirection.first) + " " + status.findStatusMessage() +  "</title>\n"
        "   </head>\n"
        "   <body>\n"
        "       <p>\n"
        "           Content moved to : " + redirection.second + "\n"
        "       </p>\n"
        "   </body>\n"
        "</html>\n";

    return redirectionPage;
}


std::string     buildErrorHtmlPage(std::string content)
{
    std::string     errorPage;

    errorPage =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "   <head>\n"
        "       <title> Error " + content +  "</title>\n"
        "   </head>\n"
        "   <body>\n"
        "       <p>\n"
        "           Error : " + content + "\n"
        "       </p>\n"
        "   </body>\n"
        "</html>\n";

    return errorPage;
}


}; // namespace html