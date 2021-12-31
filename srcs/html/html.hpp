#ifndef HTML_HPP
# define HTML_HPP

#include "webserv.hpp"
#include "file.hpp"
#include "httpStatus.hpp"
#include "utils.hpp"

namespace html {

std::string     buildPage(std::string message);
std::string     buildAutoIndexPage(std::string path);
std::string     buildRedirectionPage(std::pair<int, std::string> redirection);
std::string     buildErrorHtmlPage(std::string content);


}; // namespace html

#endif