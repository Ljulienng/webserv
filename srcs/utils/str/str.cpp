#include "str.hpp"

void			capitalizeString(std::string &str)
{
	size_t i = 0;

	str[i] = toupper(str[i]);
	// while (i != std::string::npos)
	// {
	// 	for (; str[i] != '-' ; i++);
	// 	str[i] = toupper(str[i]);
	// 	i++;
	// }
}

std::string		&removeChar(std::string &str, char c)
{
	str.erase(remove(str.begin(), str.end(), c), str.end());
	return str;
}

std::string		&trimChar(std::string &str)
{
	str = removeChar(str, ' ');
	str = removeChar(str, '\n');
	str = removeChar(str, '\r');
	return str;
}

std::list<std::string>	tokenize(std::string str, char delim)
{
	std::list<std::string>	tokens;

	for (size_t i = 0; i < str.size(); i++)
	{
		while (str[i] == delim  && i < str.size())
			i++;
		size_t start = i;
		while (str[i] != delim && i < str.size())
			i++;
		tokens.push_back(str.substr(start, i - start));
	}
	return tokens;
}


bool Str::_isDelim(char c, std::string delimiters) 
{
	for (size_t i = 0; i < delimiters.size(); i++)
	{
		if (delimiters[i] == c)
			return true;
	}
	return false;
}

/*
** split string to get tokens
*/

void	Str::_tokenize(std::string delimiters)
{
	for (size_t i = 0; i < _string.size(); i++)
	{
		while (_isDelim(_string[i], delimiters) && i < _string.size())
			i++;
		size_t start = i;
		while (!_isDelim(_string[i], delimiters) && i < _string.size())
			i++;
		_tokens.push_back(_string.substr(start, i - start));
	}
}

size_t	Str::getNum()
{
	if (_tokens.size() != 1)
		throw (std::string("ValNum not found"));
	if (_string.find_first_of("0123456789") == std::string::npos) 
		throw (std::string("ValNum not found"));
	return atoi(_string.c_str());
}


std::string		&Str::getStr()
{ return _string; }

std::vector<std::string>	&Str::getTokens()
{ return _tokens; }

void	Str::setStr(const std::string &str)
{ _string = str; }


/* Constructors, destructors and overloads */

Str::Str() : _string("") {}

Str::Str(const std::string &str) : _string(str)
{
	_tokenize(" \n\t");
}

Str::Str(const std::string &str, const std::string &sep) : _string(str)
{
	_tokenize(sep);
}

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