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
#include <cstddef>

// C sockets/network
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <poll.h>

// C functions
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>

// Streams
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

#define BUF_SIZE 8192
#define MAX_CONNECTIONS 10000
#define GOOD 0
#define WAIT 1
#define DISCONNECT 2
#define OTHER 3

#define ERROR -1

// colors
#define RED  	"\033[31m"
#define GRN  	"\033[32m"
#define ORG 	"\033[33m"
#define BLU  	"\033[34m"
#define RESET 	"\033[0m"

#endif