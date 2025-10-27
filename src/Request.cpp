#include "Request.hpp"

Request::Request() : _contentLength(0) {}

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
	if (!value.empty() && value[value.size() - 1] == '\r')
		value.erase(value.size() - 1);

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
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break ;
		if (!parseHeaderLine(line))
			return false;
	}

	//Last _headers line can be the _contentLength. If not, _contentLength = 0.
	std::map<std::string, std::string>::iterator it = _headers.find("Content-Length");
	if (it != _headers.end()) {
		std::istringstream	ss(it->second);
		ss >> _contentLength;
		if (!ss)
			return false;
	}
	else
		_contentLength = 0;

	//From the empty line must be the _body (if exists). We want to read only the _coontentLength size, for security reasons only.
	//ES POSIBLE QUE ESTO NO HAGA FALTA (cambiable por std::getline(stream, _body, '\0');) si Alvaro ya está protegiendo en el socket.
	if (_contentLength > 0) {
		_body.resize(_contentLength);
		stream.read(&_body[0], _contentLength);
	}

	return true;
}

const std::string&	Request::getMethod() const {
	return _method;
}

const std::string&	Request::getPath() const {
	return _path;
}

const std::string&	Request::getVersion() const {
	return _version;
}

const std::map<std::string, std::string>&	Request::getHeaders() const {
	return _headers;
}

const std::string&	Request::getBody() const {
	return _body;
}