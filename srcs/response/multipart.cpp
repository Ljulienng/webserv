#include "multipart.hpp"

void    parseMultipart(Response* response, Request &request, std::string boundary)
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

        t_multipart*   part = new t_multipart();       // one part on the multipart

        while (1)                  // parse headers
        {
            size_t start = i;
            while (!(content[i] == '\r' && content[i + 1] == '\n'))
                i++;
            if (content[i] == '\r' && content[i + 1] == '\n')
            {   
                std::string                 headerline(content.begin() + start, content.begin() + i); 
                std::vector<std::string>    headeParts = splitString(headerline, ':');
                part->headers[headeParts[0]] = headeParts[1];
                i += 2;
                if (content[i] == '\r' && content[i + 1] == '\n')
                {
                    i += 2;
                    break ;     // end of headers
                }
            }
        }

        size_t start = i;
        part->length = 0;
        while (i + boundary.size() + 4 < contentLength) // parse content
        {
            if (content[i] == '\r' && content[i + 1] == '\n'
            && content[i + 2] == '-' && content[i + 3] == '-'
			&& !std::strncmp(boundary.c_str(), reinterpret_cast<const char*>(&content[i + 4]), boundary.size()))
            {
                i += 2;
                break ;
            }
            ++i;
            ++part->length;
        }

        part->content = new unsigned char[part->length];
        for (size_t j = 0; j < part->length; j++, start++)
            part->content[j] = content[start];

        response->addMultipart(part);
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

Response*    multipart(Response* response, Request &request, t_configMatch &configMatch, std::string contentTypeHeader)
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
    parseMultipart(response, request, boundary);

    // check filename
    std::list<t_multipart*>::iterator it = response->getMultiparts().begin();
    std::string filename;
    while (it != response->getMultiparts().end())
    {
        filename = (*it)->getFilename();
        if (filename.empty())  
            return errorResponse(response, configMatch, BAD_REQUEST);
        ++it;
    }
   
    // new version : just create file before to pass in poll() to write the fd 
    if (response->setPollFdFileToWrite((configMatch.root + configMatch.server.getUploadPath() + "/" + filename), true, "") == false)
        return errorResponse(response, configMatch, INTERNAL_SERVER_ERROR);
    response->addFile();

    response->setStatus(CREATED);
	response->setContent(html::buildPage("File upload in : " + configMatch.root + configMatch.server.getUploadPath()), "text/html");

    return response;
}