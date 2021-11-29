#ifndef FILE_HPP
# define FILE_HPP

#include "webserv.hpp"

class File
{
    private :
		std::string			_filePath;
		struct stat			_fileStat;

    public :
        File();
		File(std::string filepath);
		File(const File &src);
		~File();

		File &operator=(const File &src);

		bool		isRegularFile();
		bool		canReadFile();

};

#endif