#include "multipart.hpp"

void    parseMultipart(std::list<t_multipart> &p, Request &request, std::string boundary)
{
    std::vector<unsigned char>  content(request.getBody().begin(), request.getBody().end());
    size_t                      contentLength = atoi(request.getHeader("Content-Length").c_str());
    size_t				        i = 0;

    while (i + boundary.size() + 6 <= contentLength)
    {
        i += boundary.size() + 2;   // skip boundary
        if (content[i] == '-' && content[i + 1] == '-')
        {
            i += 4;
            break ;
        }
        i += 2;

        t_multipart     part;       // one part on the multipart
        while (1)                  // parse headers
        {
            size_t start = i;
            while (!(content[i] == '\r' && content[i + 1] == '\n'))
                i++;
            if (content[i] == '\r' && content[i + 1] == '\n')
            {   
                std::string headerline(content.begin() + start, content.begin() + i); 
                std::vector<std::string>    headeParts = splitString(headerline, ':');
                part.headers[headeParts[0]] = headeParts[1];
                i += 2;
                if (content[i] == '\r' && content[i + 1] == '\n')
                {
                    i += 2;
                    break ;     // end of headers
                }
            }
        }

        part.content = &content[i];
        part.length = 0;
        while (i + boundary.size() + 4 < contentLength) // parse content
        {
            if (content[i] == '\r' && content[i + 1] == '\n'
            && content[i + 2] == '-' && content[i + 3] == '-'
			&& !::strncmp(boundary.c_str(), reinterpret_cast<const char*>(&content[i + 4]), boundary.size()))
            {
                i += 2;
                break ;
            }
            ++i;
            ++part.length;
        }
        p.push_back(part);
    }
}

std::string t_multipart::getFilename() const
{
    std::map<std::string, std::string>::const_iterator contentDisp = headers.find("Content-Disposition");
    
    if (contentDisp == headers.end())
        return "";
    size_t i = contentDisp->second.find("filename=\"");
    if (i == std::string::npos)
        return "";
    i += 10;
    size_t start = i;
    while (contentDisp->second[i] != '\"')
        i++;
    return std::string(contentDisp->second.begin() + start, contentDisp->second.begin() + i);
}

Response    multipart(Response &response, Request &request, t_configMatch &configMatch, std::string contentTypeHeader)
{
    // check that we have the 2 parts -> "Content-type: multipart/form-data; boundary"
    if (contentTypeHeader.find(";") == std::string::npos)
        return errorResponse(response, configMatch, BAD_REQUEST);
    std::string     contentType = contentTypeHeader.substr(0, contentTypeHeader.find_first_of(";"));
    std::string     boundary = contentTypeHeader.substr(contentTypeHeader.find_first_of(";") + 1, std::string::npos);

    if (boundary.substr(0, 9) != "boundary=")
        return errorResponse(response, configMatch, BAD_REQUEST);
    boundary.erase(0, 9);
    
    // check if the content is ok and get content original file (without headers and boundary)
    std::list<t_multipart> parts;
    parseMultipart(parts, request, boundary);

    // create and write content into the new file for each part
    std::list<t_multipart>::iterator it = parts.begin();
	while (it != parts.end())
    {
        std::string filename = it->getFilename();
        if (filename.empty())  
            return errorResponse(response, configMatch, BAD_REQUEST);
        appendToFile(configMatch.root + configMatch.server.getUploadPath() + "/" + filename, reinterpret_cast<char*>(it->content), it->length);
        ++it;
    }

    response.setStatus(CREATED);
	response.setContent(html::buildPage("File upload in : " + configMatch.root + configMatch.server.getUploadPath()), "text/html");

    return response;
}