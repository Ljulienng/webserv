#include <signal.h>
#include <iostream>
#include "hub.hpp"

bool g_run;

void     sigintHandler(int sig)
{
    (void)sig;
    std::cout << std::endl;
    std::cout << GRN << "... Good bye world ..." << RESET << std::endl;
    g_run = false;
}

int     main(int ac, char **argv)
{
    g_run = true;

    signal(SIGINT, sigintHandler);
    if (ac == 2)
    {
        Hub hub;
        try
        {
            hub.start(argv[1]); // parse and setup
            while (g_run)
                hub.process();           
        }
        catch(std::string error)
        {
            hub.clean();
			std::cerr << RED << error << RESET << std::endl;
        }
    }
    else
    {
        std::cerr << "Bad number of arguments, to use it -> ./webserv <configuration file>" << std::endl; 
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);  
}