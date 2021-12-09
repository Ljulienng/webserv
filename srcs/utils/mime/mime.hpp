#ifndef MIME_HPP
# define MIME_HPP

#include "webserv.hpp"

class Mime
{
    private :
        // map with the extension of the file and the corresponding mime
        std::map<std::string, std::string>  _existingMimes;

        void    _setExistingMimes();

    public :
        Mime();
		Mime(const Mime &src);
		~Mime();

        std::string     findMime(std::string extension);
};

#endif