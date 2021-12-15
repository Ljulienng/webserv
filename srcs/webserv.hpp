#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// Containers
#include <list>
#include <vector>
#include <map>
#include <set>
#include <queue>
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
#include <stdlib.h>

// Streams
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#define MAX_BUF_LEN 4096
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define MAX_CONNECTIONS 100

// colors
#define RED  	"\033[31m"
#define GRN  	"\033[32m"
#define ORG 	"\033[33m"
#define BLU  	"\033[34m"
#define RESET 	"\033[0m"

#endif