#include "str.hpp"

std::vector<char>::iterator	lastChar(std::vector<char> &str)
{
	// std::vector<char>::iterator it;
	// for (it = str.begin(); *it != 0; it++);
	// return (it);

	std::vector<char>::iterator it = str.end();
	it--;
	for ( ; it != str.begin(); it--)
		if (*it != 0)
			break ;
	return (++it);
}

char	lastChar(std::string &str)
{
	size_t i;
	// for (i = 0; str[i] != 0; i++);
	// return (str[i - 1]);

	for (i = str.length(); str[i] != 0 || i == 0; i--);
	return str[i];

}

std::vector<std::string>	splitString(std::string &str, char c)
{
	std::istringstream			strStream(str);
	std::string					tmp;
	std::vector<std::string>	strVector;

	while(std::getline(strStream, tmp, c))
		strVector.push_back(tmp);
	return (strVector);
}

void						capitalizeString(std::string &str)
{
	size_t i = 0;

	str[i] = toupper(str[i]);
}

std::string					&removeChar(std::string &str, char c)
{	
	str.erase(remove(str.begin(), str.end(), c), str.end());
	return str;
}

std::string					&trimChar(std::string &str)
{
	str = removeChar(str, ' ');
	str = removeChar(str, '\n');
	str = removeChar(str, '\r');
	return str;
}

/* split string to get tokens */
std::vector<std::string>	tokenize(std::string str, std::string delim)
{
	std::vector<std::string>	tokens;

	for (size_t i = 0; i < str.size(); i++)
	{
		while (isDelim(str[i], delim)  && i < str.size())
			i++;
		size_t start = i;
		while (!isDelim(str[i], delim) && i < str.size())
			i++;
		tokens.push_back(str.substr(start, i - start));
	}
	return tokens;
}

bool isDelim(char c, std::string delimiters)
{
	for (size_t i = 0; i < delimiters.size(); i++)
		if (delimiters[i] == c)
			return true;
	return false;
}

size_t		Str::getNum()
{
	if (_tokens.size() != 1)
		throw (std::string("Error: need only one token"));
	if (_string.find_first_of("0123456789") == std::string::npos) 
		throw (std::string("Error: data not well formatted"));
	return atoi(_string.c_str());
}

std::string     myItoa(size_t number)
{
	std::stringstream   stream;

	stream << number;
	return stream.str();
}

std::string		&Str::getStr()
{ return _string; }

std::vector<std::string>	&Str::getTokens()
{ return _tokens; }

void	Str::setStr(const std::string &str)
{ _string = str; }


/* Constructors, destructors and overloads */

Str::Str() : _string("") {}

Str::Str(const std::string &str) : 
	_string(str),
	_tokens(tokenize(str, " \t\n"))
{}

Str::Str(const std::string &str, const std::string &sep) : 
	_string(str),
	_tokens(tokenize(str, sep))
{}

Str::Str(const Str &src)
{
	*this = src;
}

Str::~Str() {}

Str &Str::operator=(const Str &src)
{
	if (&src != this)
	{
		_string = src._string;
		_tokens = src._tokens;
		// to be completed
	}
	return (*this);
}