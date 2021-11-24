#ifndef STR_HPP
# define STR_HPP

#include <vector>
#include <string>
#include <sstream>

class Str
{
    private :
		std::string					_string;
		std::vector<std::string>	_tokens;

    public :
        Str();
		Str(const std::string &str);
		Str(const Str &src);
		~Str();

		Str &operator=(const Str &src);

		void						tokenize();
		size_t						getNum();
		std::string					&getStr();
		std::vector<std::string>	&getTokens();
		void						setStr(const std::string &str);
};

#endif