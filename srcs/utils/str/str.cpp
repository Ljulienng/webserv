#include "str.hpp"

/*
** split string to get tokens
*/
void	Str::tokenize()
{
	for (size_t i = 0; i < _string.size(); i++)
	{
		while ((_string[i] == ' ' || _string[i] == '\t' || _string[i] == '\n') && i < _string.size())
			i++;
		size_t start = i;
		while (_string[i] != ' ' && _string[i] != '\t' && _string[i] != '\n' && i < _string.size())
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
	tokenize();
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
		// to be completed
	}
	return (*this);
}