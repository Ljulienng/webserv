#ifndef FILE_HPP
# define FILE_HPP

#include "webserv.hpp"
#include "mime.hpp"

/*
** useful functions to handle files
*/
class File
{
    private :
		std::string			_filePath;
		std::string			_fileContent;
		struct stat			_fileStat;

    public :
        File();
		File(std::string filepath);
		File(const File &src);
		~File();
		File &operator=(const File &src);

		bool		isRegularFile();
		bool		isDirectory();
		bool		canReadFile();
		std::string	findContentType(std::string extension);
		// ...

		// GETTERS
		std::string		&getFilePath();
		std::string		&getFileContent();
		struct stat		&getfileStat();
};

#endif