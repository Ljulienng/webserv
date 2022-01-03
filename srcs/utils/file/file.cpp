#include "file.hpp"

bool	File::exists()
{
	if (stat(_filePath.c_str(), &_fileStat) < 0)
		return false;
	return true;
}

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

bool 	File::createFile(std::string filename)
{
	int fd;

	if ((fd = open(filename.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666)) < 0)
		return false;
	close(fd);

	return true;
}

bool	File::createFile(std::string filename, std::string content)
{
	int fd;

	if ((fd = open(filename.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666)) < 0)
		return false;
	write(fd, content.c_str(), content.size());
	close(fd);

	return true;	
}


/* build the list of files in a directory */
std::list<std::string>		File::buildFilesList()
{
	DIR 					*dir;
	struct 	dirent 			*diread;
	std::list<std::string>	files;

	if ((dir = opendir(_filePath.c_str())) != NULL)
		while ((diread = readdir(dir)) != NULL)
			files.push_back(diread->d_name);
	closedir(dir);

	return files;
}

std::string	File::findContentType(std::string extension)
{
	Mime mime;

	return (mime.findMime(extension));
}


/* GETTERS */
std::string		&File::getFilePath()
{ return _filePath; }

std::string		&File::getFileContent()
{ return _fileContent; }

struct stat		&File::getfileStat()
{ return _fileStat; }


/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
File::File() :
	_filePath(),
	_fileContent(),
	_fileStat() {}

File::File(std::string filePath) : 
	_filePath(filePath),
	_fileContent(),
	_fileStat()
{
	if (isRegularFile())
	{
		std::ifstream ifs;
		ifs.open(filePath.c_str());
		std::getline(ifs, _fileContent, '\0');
		ifs.close();
	}
}

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