#include "httpStatus.hpp"

std::string     HttpStatus::findStatusMessage(size_t code)
{
	std::map<size_t, std::string>::iterator	it = _existingStatus.find(code);

	if (it != _existingStatus.end())
		return it->second;
	return "";
}

void    HttpStatus::_setExistingStatus()
{
	// INFORMATION
	_existingStatus[100] = "Continue";
	_existingStatus[101] = "Switching Protocols";
	_existingStatus[102] = "Processing";
	_existingStatus[103] = "Early Hints";

	// SUCCESS
	_existingStatus[200] = "OK";
	_existingStatus[201] = "Created";
	_existingStatus[202] = "Accepted";
	_existingStatus[203] = "Non-Authoritative Information";
	_existingStatus[204] = "No Content";
	_existingStatus[205] = "Reset Content";
	_existingStatus[206] = "Partial Content";
	_existingStatus[207] = "Multi-Status";
	_existingStatus[208] = "Already Reported";
	_existingStatus[210] = "Content Different";
	_existingStatus[216] = "IM Used";

	// REDIRECTION
	_existingStatus[300] = "Multiple Choices";
	_existingStatus[301] = "Moved Permanently";
	_existingStatus[302] = "Found";
	_existingStatus[303] = "See Other";
	_existingStatus[304] = "Not Modified";
	_existingStatus[305] = "Use Proxy ";
	_existingStatus[306] = "Switch Proxy";
	_existingStatus[307] = "Temporary Redirect";
	_existingStatus[308] = "Permanent Redirect";
	_existingStatus[310] = "Too many Redirects";

	// ERROR HTTP CLIENT
	_existingStatus[400] = "Bad Request";
	_existingStatus[401] = "Unauthorized";
	_existingStatus[402] = "Payment Required";
	_existingStatus[403] = "Forbidden";
	_existingStatus[404] = "Not Found";
	_existingStatus[405] = "Method Not Allowed";
	_existingStatus[406] = "Not Acceptable";
	_existingStatus[407] = "Proxy Authentication Required";
	_existingStatus[408] = "Request Time-out";
	_existingStatus[409] = "Conflict";
	_existingStatus[410] = "Gone";
	_existingStatus[411] = "Length Required";
	_existingStatus[412] = "Precondition Failed";
	_existingStatus[413] = "Request Entity Too Large";
	_existingStatus[414] = "Request-URI Too Long";
	_existingStatus[415] = "Unsupported Media Type";
	_existingStatus[416] = "Requested range unsatisfiable";
	_existingStatus[417] = "Expectation failed";
	_existingStatus[418] = "I’m a teapot";
	_existingStatus[421] = "Bad mapping / Misdirected Request";
	_existingStatus[422] = "Unprocessable entity";
	_existingStatus[423] = "Locked";
	_existingStatus[424] = "Method failure";
	_existingStatus[425] = "Too Early";
	_existingStatus[426] = "Upgrade Required";
	_existingStatus[428] = "Precondition Required";
	_existingStatus[429] = "Too Many Requests";
	_existingStatus[431] = "Request Header Fields Too Large";
	_existingStatus[449] = "Retry With";
	_existingStatus[450] = "Blocked by Windows Parental Controls";
	_existingStatus[451] = "Unavailable For Legal Reasons";
	_existingStatus[456] = "Unrecoverable Error";

	// ERROR SERVER
	_existingStatus[500] = "Internal Server Error";
	_existingStatus[501] = "Not Implemented";
	_existingStatus[502] = "Bad Gateway ou Proxy Error";
	_existingStatus[503] = "Service Unavailable";
	_existingStatus[504] = "Gateway Time-out";
	_existingStatus[505] = "HTTP Version not supported";
	_existingStatus[506] = "Variant Also Negotiates";
	_existingStatus[507] = "Insufficient storage";
	_existingStatus[508] = "Loop detected";
	_existingStatus[509] = "Bandwidth Limit Exceeded";
	_existingStatus[510] = "Not extended";
	_existingStatus[511] = "Network authentication required";
}


/* GETTERS */
size_t          &HttpStatus::getCode()
{
	return _code;
}

std::string     &HttpStatus::getMessage()
{
	return _message;
}

/* CONSTRUCTORS, DESTRUCTOR AND OVERLOADS */
HttpStatus::HttpStatus() : _code(), _message()
{
	_setExistingStatus();
}

HttpStatus::HttpStatus(size_t code) : _code(code)
{
	_setExistingStatus();
	_message = findStatusMessage(_code);
}

HttpStatus::HttpStatus(const HttpStatus &src)
{
	*this = src;
}

HttpStatus::~HttpStatus() {}


HttpStatus &HttpStatus::operator=(const HttpStatus &src)
{
	if (&src != this)
	{
		_existingStatus = src._existingStatus;
		_code = src._code;
		_message = src._message;
		// to be completed if new attributes
	}
	return (*this);
}