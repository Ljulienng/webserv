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

class Response
{
    public:
        enum fileState
        {
            NONE,
            DATATOREAD,
            DATATOWRITE
        };

    private :
        std::map<std::string, std::string>  _headers;
        std::string                         _httpVersion;
        HttpStatus                          _httpStatus;
        std::vector<unsigned char>          _content;
        std::string                         _message; // the formatted message to insert to the buffer

        // manage files to read file (get) or append to file (post)
        struct pollfd                       _pollFdFile;
        int                                 _stateFile;
        int                                 _indexFile;
        std::list<t_multipart*>             _multiparts;
        bool                                _isMultipart;
        std::string                         _contentToCopyInFile;

        void                                _updateMessage();

    public :
		Response();
		Response(const Response &src);
		~Response();
        Response &operator=(const Response &src);	

        void                    addMultipart(t_multipart* part);
        int                     readFile(bool *endOfResponse, bool *endToReadFile);
        int                     writeFile(bool *endOfResponse, bool *endToWriteFile);
        void 	                addFile();
        void 	                deleteFile();
        void                    endToReadorWrite();

        // SETTERS
        void                    setHeader(std::string key, std::string value);
        void                    setContent(std::vector<unsigned char> content, std::string contentType);
        void                    setContentType(std::string contentType);
        void                    setContentLength();
        void                    setStatus(int status);
        void                    setIndexFile(int indexFile); // index to find the file in _fds[]
        bool                    setPollFdFileToRead(const char *file);
        bool                    setPollFdFileToWrite(std::string file, bool multipart, std::string content);
        
        // GETTERS
        std::map<std::string, std::string>  &getHeaders();
        std::string                         getHeader(std::string key);
        std::string                         &getHttpVersion();
        HttpStatus                          &getHttpStatus();
        std::vector<unsigned char>          &getContent();
        std::string                         &getMessage();
        struct pollfd                       &getPollFdFile();
        int                                 &getStateFile();
        int                                 &getIndexFile();
        std::list<t_multipart*>             &getMultiparts();

};

#include "multipart.hpp"

#endif