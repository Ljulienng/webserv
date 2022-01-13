#ifndef HTML_HPP
# define HTML_HPP

#include "webserv.hpp"
#include "file.hpp"
#include "httpStatus.hpp"
#include "utils.hpp"

namespace html {

std::vector<unsigned char>     buildPage(std::string message);
std::vector<unsigned char>     buildAutoIndexPage(std::string path);
std::vector<unsigned char>     buildRedirectionPage(std::pair<int, std::string> redirection);
std::vector<unsigned char>     buildErrorHtmlPage(std::string content);

}; // namespace html

#endif