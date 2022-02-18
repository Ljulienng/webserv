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
typedef struct s_multipart t_multipart;

/******* test ********/
enum fileState
{
    NONE,
    DATATOREAD,
    DATATOWRITE
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
        std::string                         _fileToWriteIn;
        int                                 _stateFile;
        int                                 _indexFile;
        std::string                         _bodyRequestToPost; // post : stocke au depart puis on la copie dans le fichier cree apres etre passe dans poll
        std::vector<unsigned char>          _bodyRequestToPostVector;
        std::pair<char*,size_t>             _bodyRequestToPostchar;
        std::list<t_multipart*>             _multiparts;
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
        void                                setContentType(std::string contentType);
        void                                setContentLength();
        void                                setStatus(int status);
        
        // GETTERS
        std::map<std::string, std::string>  &getHeaders();
        std::string                         getHeader(std::string key);
        std::string                         &getHttpVersion();
        HttpStatus                          &getHttpStatus();
        std::vector<unsigned char>          &getContent();
        std::string                         &getMessage();


        /******* test ********/
        void                readFile(bool *endOfResponse, bool *endToReadFile);
        void                writeFile(bool *endOfResponse, bool *endToWriteFile);
        void 	            addFile();
        void 	            deleteFile();

        void                setIndexFile(int indexFile); // index pour retrouver le file dans _fds[]
        bool                setPollFdFileToRead(const char *file);
        bool                setPollFdFileToWrite(std::string file, /*std::string*/ /*std::vector<unsigned char>*/std::pair<char*,size_t>  bodyRequestToPost);
        void                endToReadorWrite();
        void                setMultiparts(std::list<t_multipart*> parts);
        struct pollfd       getPollFdFile();
        int                 getStateFile();
        int                 getIndexFile();
        /********************/

};

#include "multipart.hpp"

#endif