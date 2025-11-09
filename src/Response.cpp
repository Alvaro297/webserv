#include "../inc/Response.hpp"

Response::Response() : _statusCode(0), _error(0) {}

void	Response::setStatus(int code, const std::string& msg) {
	_statusCode = code;
	_statusMessage = msg;
}

void	Response::setHeader(const std::string& key, const std::string& value) {
	_headers[key] = value;
}

void	Response::setBody(const std::string& body) {
	_body = body;
}

std::string	Response::genResponseString() {
	std::ostringstream	resp;

	resp << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n"; //First line (status info)
	
	for (std::map<std::string, std::string>::iterator iter = _headers.begin(); iter != _headers.end(); ++iter) //Headers lines (one for line)
		resp << iter->first << ": " << iter->second << "\r\n";

	if (_headers.find("Content-length") == _headers.end()) //If the headers don't already include the body length we have to add it.
		resp << "Content-length: " << _body.size() << "\r\n";
	
	resp << "\r\n"; //Separator between headers and body

	resp << _body; //Body

	return resp.str();
}

const std::string&	Response::getBody() const {
	return _body;
}

int	Response::getError() const {
	return _error;
}
