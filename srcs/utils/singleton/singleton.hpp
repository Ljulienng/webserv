#ifndef SINGLETON_HPP
# define SINGLETON_HPP

#include "webserv.hpp"

template <typename T>
class Singleton
{
    protected :
        Singleton() {};
        Singleton(const Singleton & copy) { (void)copy; };
        Singleton &operator=(const Singleton &ope) { (void)ope; };
		
    public :

        static T &getInstance()
        {
            static T _instance;
            return _instance;
        }
};

#endif