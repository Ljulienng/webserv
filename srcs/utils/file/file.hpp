#ifndef FILE_HPP
# define FILE_HPP

#include "webserv.hpp"
#include "mime.hpp"

#define BUF_SIZE 512
/*
** useful functions to handle files and directories
*/
class File
{
    private :
		std::string					_filePath;
		std::vector<unsigned char>	_fileContent;
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
		void						readFile();
		bool						fileIsInDirectory(std::string filename);

		// GETTERS
		std::string					&getFilePath();
		std::vector<unsigned char>	&getFileContent();
		struct stat					&getfileStat();
};

void 			appendToFile(const std::string &path, const char *content, size_t n);
std::string     getExtension(std::string filename);
// bool			cmp_by_filename(const std::string &a, const std::string &b);

#endif