#ifndef STR_HPP
# define STR_HPP

#include "webserv.hpp"

class Str
{
    private :
		std::string					_string;
		std::vector<std::string>	_tokens;

    public :
        Str();
		Str(const std::string &str);
		Str(const std::string &str, const std::string &sep);
		Str(const Str &src);
		~Str();
		Str &operator=(const Str &src);

		size_t						getNum();
		std::string					&getStr();
		std::vector<std::string>	&getTokens();
		void						setStr(const std::string &str);
};

// std::vector<char>::iterator	lastChar(std::vector<char> &str);
// char						lastChar(std::string &str);
std::vector<std::string>	splitString(std::string &str, char c);
void						capitalizeString(std::string &str);
std::string					&removeChar(std::string &str, char c);
std::string					&trimChar(std::string &str);
bool 						isDelim(char c, std::string delimiters);
std::vector<std::string>	tokenize(std::string str, std::string delim);
std::string    				myItoa(size_t number);

#endif