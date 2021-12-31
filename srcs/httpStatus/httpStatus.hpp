#ifndef HTTPSTATUS_HPP
# define HTTPSTATUS_HPP

#include "webserv.hpp"

class HttpStatus
{
    private :
        // map with the status code and the corresponding status message
        std::map<size_t, std::string>   _existingStatus;
        size_t                          _code;
        std::string                     _message;

        void    _setExistingStatus();

    public :
        HttpStatus();
        HttpStatus(size_t code);
        HttpStatus(const HttpStatus &src);
        ~HttpStatus();
        HttpStatus &operator=(const HttpStatus &src);	

        std::string     findStatusMessage();

        // SETTERS
        void            setStatus(int status);
        
        // GETTERS
        size_t          &getCode();
        std::string     &getMessage();
};

#endif