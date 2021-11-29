//Main file

#include <signal.h>
#include <iostream>
#include "hub.hpp"

#define RED  "\033[31m"

bool    g_run;

void     sigintHandler(int sig)
{
    (void)sig;
    g_run = false;
}

int     main(int ac, char **argv, char **env)
{
    (void)env; // useful for cgi part
    g_run = true;

    signal(SIGINT, sigintHandler);
    if (ac == 2)
    {
        try
        {
			Hub hub(argv[1]);
            hub.start(); // parse and setup
            hub.process();
            hub.getConfig().debug();
			//hub.suiteduprogramme();
        }
        catch(std::string error)
        {
			std::cerr << RED << error << std::endl;
        }
    }
    else
    {
        std::cerr << "Bad number of arguments, to use it -> ./webserv <configuration file>" << std::endl; 
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);  
}