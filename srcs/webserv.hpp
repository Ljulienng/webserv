#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// Containers
#include <list>
#include <vector>
#include <map>
#include <set>
#include <iterator>
#include <utility>

// Others
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <sys/types.h>
#include <errno.h>
#include "utils/str/str.hpp"

// C sockets/network
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>

// C functions
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

// Streams
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#define MAX_BUF_LEN 4096
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define MAX_CONNECTIONS 100

// NON MEMBER FUNCTIONS
int isValidExpression(std::string expression, const char **validExpressions);

static const char* configExpression[] = {
	"cgi",
	"max_body_size",
	"error_pages",
	"server",
	0
};

static const char* serverExpression[] = {
	"server_name",
	"ip",
	"port",
	"max_body_size",
	"location",
	0
};

static const char* locationExpression[] = {
	"path",
	"root",
	"accepted_methods",
	"default_file",
	"autoindex",
	"max_body_size",
	"redirection",
	0
};

#endif