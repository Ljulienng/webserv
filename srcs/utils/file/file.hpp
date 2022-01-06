#ifndef FILE_HPP
# define FILE_HPP

#include "webserv.hpp"
#include "mime.hpp"

/*
** useful functions to handle files and directories
*/
class File
{
    private :
		std::string					_filePath;
		std::string					_fileContent;
		struct stat					_fileStat;

    public :
        File();
		File(std::string filepath);
		File(const File &src);
		~File();
		File &operator=(const File &src);

		bool						exists();
		bool						isRegularFile();
		bool						isDirectory();
		bool						canReadFile();
		bool						createFile(std::string filename);
		bool						createFile(std::string filename, std::string content);
		std::list<std::string>		buildFilesList();
		std::string					findContentType(std::string extension);
		// ...

		// GETTERS
		std::string		&getFilePath();
		std::string		&getFileContent();
		struct stat		&getfileStat();
};

#endif