#include "file.hpp"

bool	File::isRegularFile()
{
	if (stat(_filePath.c_str(), &_fileStat) < 0)
		return false;
	return (S_ISREG(_fileStat.st_mode));
}


bool	File::isDirectory()
{
	if (stat(_filePath.c_str(), &_fileStat) < 0)
		return false;
	return (S_ISDIR(_fileStat.st_mode));
}

bool	File::canReadFile()
{
	std::ifstream	fileStream;
	fileStream.open(_filePath.c_str(), std::ios::in);
	if (fileStream.fail())
		return false;
	fileStream.close();
	return true;
}

std::string	File::findContentType(std::string extension)
{
	Mime mime;

	return (mime.findMime(extension));
}


/* GETTERS */
std::string		&File::getFilePath()
{
	return _filePath;
}

struct stat		&File::getfileStat()
{
	return _fileStat;
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */

File::File() : _filePath(), _fileStat() {}

File::File(std::string filePath) : _filePath(filePath), _fileStat() {}

File::File(const File &src)
{
	*this = src;
}

File::~File() {}

File &File::operator=(const File &src)
{
	if (&src != this)
	{
		_filePath = src._filePath;
		_fileStat = src._fileStat;
	}
	return (*this);
}