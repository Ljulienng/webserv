#ifndef STR_HPP
# define STR_HPP

#include "webserv.hpp"

class Str
{
    private :
		std::string					_string;
		std::vector<std::string>	_tokens;

		bool 						_isDelim(char c, std::string delimiters);
		void						_tokenize(std::string delimiters);

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
		// std::string 				capitalizeString(std::string &str);
		
};

void			capitalizeString(std::string &str);
std::string		&removeChar(std::string &str, char c);
std::string		&trimChar(std::string &str);

#endif