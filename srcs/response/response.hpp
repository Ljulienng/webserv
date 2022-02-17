#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"
#include "httpStatus.hpp"
#include "request.hpp"
#include "html.hpp"
#include "str.hpp"
#include "log.hpp"
#include "server.hpp"

class Configuration;

/******* test ********/
enum fileState
{
    NONE,
    DATATOREAD,
    ENDTOREAD
};
/********************/

class Response
{
    private :
        std::map<std::string, std::string>  _headers;
        std::string                         _httpVersion;
        HttpStatus                          _httpStatus;
        std::vector<unsigned char>          _content;
        std::string                         _message; // the formatted message to insert to the buffer

        /******* test ********/
        // manage files to read file (get) or append to file (post)
        struct pollfd                       _pollFdFile;
        int                                 _stateFile;
        /********************/

        void        _updateMessage();

    public :
		Response();
		Response(const Response &src);
		~Response();
        Response &operator=(const Response &src);	

        // SETTERS
        void                                setHeader(std::string key, std::string value);
        void                                setContent(std::vector<unsigned char> content, std::string contentType);
        void                                setStatus(int status);
        
        // GETTERS
        std::map<std::string, std::string>  &getHeaders();
        std::string                         getHeader(std::string key);
        std::string                         &getHttpVersion();
        HttpStatus                          &getHttpStatus();
        std::vector<unsigned char>          &getContent();
        std::string                         &getMessage();


        /******* test ********/
        void 	            addFile();
        void                setPollFd(struct pollfd newPollFd);
        void                setPollFdFileToRead(const char *file);
        void                setStateFile(int state);
        struct pollfd       getPollFdFile();
        int                 &getStateFile();
        /********************/

};

#endif