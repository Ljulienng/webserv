#ifndef FILE_HPP
# define FILE_HPP

#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

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