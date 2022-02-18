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

// (! write pas dans poll())
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

	// files.sort(cmp_by_filename);
	return files;
}

std::string	File::findContentType(std::string extension)
{
	Mime mime;

	return (mime.findMime(extension));
}

/*
** if we call this function, we are sure that _filepath is a directory
*/
bool	File::fileIsInDirectory(std::string filename)
{
	std::list<std::string> listFiles = buildFilesList();
	std::list<std::string>::iterator begin = listFiles.begin();
	for ( ; begin != listFiles.end(); ++begin)
		if (*begin == filename)
			return true;
	return false;
}


/* GETTERS */
std::string		&File::getFilePath()
{ return _filePath; }

std::vector<unsigned char>	&File::getFileContent()
{ return _fileContent; }

struct stat		&File::getfileStat()
{ return _fileStat; }

void	File::openFile()
{
	// char	buf[BUF_SIZE + 1] = {0};
	// int 	fd;
	// size_t	i;
	// size_t	ret;
	
	// fd = open(_filePath.c_str(), O_RDONLY);
	// if (fd < 0)
	// 	return ;
	// while ((ret = read(fd, buf, BUF_SIZE)) > 0)
	// {
	// 	for (size_t j = 0; j < ret; ++j)
	// 		_fileContent.push_back(buf[j]);
	// 	i = 0;
	// 	while (i < BUF_SIZE)
	// 		buf[i++] = 0;
	// }
	// if (ret < 0)
	// 	return ;
	// close(fd);

	std::ifstream 	file;
	file.open(_filePath.c_str(), std::ios::binary);
	if (file.is_open())
	{
		while (file.good())
			_fileContent.push_back(file.get());
		_fileContent.pop_back(); // erase only the last char (don't ask why ...)
		file.close();
	}
}

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
		openFile();
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
		_fileContent = src._fileContent;
		_fileStat = src._fileStat;
	}
	return (*this);
}

/* NON MEMBERS*/
void 	appendToFile(const std::string &path, const char *content, size_t n)
{
	std::ofstream file;
	file.open(path.c_str(), std::ofstream::binary);
	file.write(content, n);
	file.close();

	//V2
	// (void)n;
	// std::string toAppend(content, 0, n);
	// std::ofstream file;
	// file.open(path.c_str(), std::ofstream::binary);
	// if (file.is_open())
	// {
	// 	file << toAppend;
	// 	file.close();
	// }
}

void 	appendToFileBis(const std::string &path, const char *content, size_t n)
{
	FILE * pfile;
	pfile = fopen(path.c_str(), "w+");
	std::cerr << "[appendToFile] fd of postFile = " << fileno(pfile) << "\n";
	for (size_t i = 0; i < n; i++)
		fputc(content[i], pfile);
	fclose(pfile);
}

std::string     getExtension(std::string filename)
{
    size_t index = filename.find_last_of(".") + 1;
    if (index == std::string::npos)
        return NULL;
    return (filename.substr(index, std::string::npos));
}

// bool			cmp_by_filename(const std::string &a, const std::string &b)
// { return (a < b); }