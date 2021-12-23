#ifndef HTML_HPP
# define HTML_HPP

#include "webserv.hpp"
#include "file.hpp"

namespace html {

std::string     buildAutoIndexPage(std::string path);
std::string     buildErrorHtmlPage(std::string content);


}; // namespace html

#endif