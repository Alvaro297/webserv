#include "Request.hpp"

Request::Request() {}

//Parse to separate the key/value of the _headers
bool	Request::parseHeaderLine(std::string line) {
	size_t	colon = line.find(':');
	if (colon == std::string::npos)
		return false;

	std::string	key = line.substr(0, colon);
	std::string value = line.substr(colon + 1);

	//Removing ' ' and '\r'
	if (!value.empty() && value[0] == ' ')
		value.erase(0, 1);
	if (!value.empty() && value.back() == '\r')
		value.pop_back();

	_headers[key] = value;
	return true;
}

//Parse the Request to fill the vars.
bool	Request::parseRequestValidity(const std::string& rawReq) {
	std::istringstream  stream(rawReq);
	std::string  line;

	if (!std::getline(stream, line))
		return false;

	//The first line must include _method, _path and _version.
	std::istringstream	ss(line);
	if (!(ss >> _method >> _path >> _version))
		return false;

	//From second line to empty line must be the _headers.
	while (std::getline(stream, line)) {
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.empty())
			break ;
		if (!parseHeaderLine(line))
			return false;
	}

	//From the empty line must be the _body (if exists).
	std::getline(stream, _body, '\0');

	return true;
}

const std::string&	Request::getMethod() const {return _method;}

const std::string&	Request::getPath() const {return _path;}

const std::string&	Request::getVersion() const {return _version;}

const std::map<std::string, std::string>&	Request::getHeaders() const {return _headers;}

const std::string&	Request::getBody() const {return _body;}