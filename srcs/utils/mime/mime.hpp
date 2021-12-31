#ifndef MIME_HPP
# define MIME_HPP

#include "webserv.hpp"

class Mime
{
    private :
        // map with the extension of the file and the corresponding mime
        std::map<std::string, std::string>  _existingMimes;
        std::string                         _mime;

        void    _setExistingMimes();

    public :
        Mime();
        Mime(std::string extension);
		Mime(const Mime &src);
		~Mime();

        std::string     findMime(std::string extension);
        std::string     &getMime();
};

#endif