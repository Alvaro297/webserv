#include "../inc/Request.hpp"

	//CONSTRUCTOR//---------------->
Request::Request() : _contentLength(0) {}
//----------------<


	//MAIN PARSER//---------------->
// Parse the Request to fill the vars.
bool	Request::parseRequestValidity(const std::string& rawReq) {
	std::istringstream  stream(rawReq);
	std::string  line;

	if (!std::getline(stream, line))
		return false;

	//The first line must include _method, _path and _version.
	std::istringstream	ss(line);
	if (!(ss >> _method >> _path >> _version))
		return false;
	if (!parsePath())
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
	if (_contentLength > 0) {
		_body.resize(_contentLength);
		stream.read(&_body[0], _contentLength);
	}

	return true;
}
//----------------<


	//PATH PARSERS//---------------->
// Parse the path in case there are query strings on it (separating actual _path and _queryMap).
bool	Request::parsePath() {
	try {
		size_t	queryPos = _path.find("?");
		if (queryPos != std::string::npos) {
			if (!parseQuery(_path.substr(queryPos + 1)))
				return false;
			_path.erase(queryPos);
		}
		setFileType();
	}
	catch (const std::exception& e) {
		return false;
	}
	return true;
}

//Determine the type of file and save it for later use.
bool	Request::setFileType() {
	try {
		size_t pos = _path.rfind('.');
	
		if (pos == std::string::npos) {
			_fileType = "application/octet-stream";
			return true;
		}
	
		std::string ext = _path.substr(pos);
	
		static const std::map<std::string, std::string>	typeMap = {
			std::make_pair(".html", "text/html"),
			std::make_pair(".htm", ".htm"),
			std::make_pair(".css", "text/css"),
			std::make_pair(".js", "application/javascript"),
			std::make_pair(".json", "application/json"),
			std::make_pair(".png", "image/png"),
			std::make_pair(".jpg", "image/jpeg"),
			std::make_pair(".jpeg", "image/jpeg"),
			std::make_pair(".gif", "image/gif"),
			std::make_pair(".svg", "image/svg+xml"),
			std::make_pair(".txt", "text/plain"),
			std::make_pair(".xml", "application/xml"),
		};
	
		std::map<std::string, std::string>::const_iterator it = typeMap.find(ext);
		if (it != typeMap.end())
			_fileType = it->second;
			return true;
	
		_fileType = "application/octet-stream";
	}
	catch (const std::exception& e) {
		return false;
	}
	return true;
}

// Separate the different queryParams and insert them in the _queryMap. & is the separator between different params.
bool	Request::parseQuery(std::string rawQuery) {
	try {
		while (!rawQuery.empty()) {
			size_t		ampers = rawQuery.find("&");
			std::string	param = rawQuery.substr(0, ampers);
			size_t		equal = param.find("=");

			if (equal != std::string::npos) {
				std::string	key = param.substr(0, equal);
				std::string value = param.substr(equal + 1);
				_queryMap[key] = value;
			}
			else if (!param.empty())
				_queryMap[param] = "";

			if (ampers != std::string::npos)
				rawQuery.erase(0, ampers + 1);
			else
				rawQuery.clear();
		}
	}
	catch (const std::exception& e) {
		return false;
	}
	return true;
}
//----------------<


	//HEADER LINES PARSERS//---------------->
//Parse to separate the key/value of the _headers
bool	Request::parseHeaderLine(std::string& line) {
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

	//Check if it is a multipart header and parse it in such the case (parseMultipart parses header and body at once).
	if (key == "Content-Type")
		if (value.find("multipart/form-data") != std::string::npos)
			if (!parseMultipart(value));
				return false;

	return true;
}

//Split the _body in the different parts, separated by _bound, and set _multiBody with them.
bool	Request::parseMultipart(std::string& multi) {
	size_t startPart = 0;
	size_t endPart = 0;

	if (!findBoundary(multi))
		return false;

	while (endPart = _body.find(_bound, startPart) != std::string::npos) {
		std::string newPart = _body.substr(startPart, endPart - startPart);
		if (!newPart.empty())
			_multiBody.push_back(newPart);
		startPart = endPart + _bound.size();
	}
	return true;
}

//Set the _bound if it's valid.
bool	Request::findBoundary(std::string& multi) {
	size_t	pos = multi.find("boundary=");

	if (pos == std::string::npos)
		return false;
	_bound = multi.substr(pos + 9);
		return true;
}
//----------------<


	//GETTERS//---------------->
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

const std::string&	Request::getQueryParam(const std::string& key) const {
	static const std::string empty = "";

	std::map<std::string, std::string>::const_iterator it = _queryMap.find(key);
	if (it != _queryMap.end())
		return it->second;
	return empty;
}

const std::string&  Request::getBound() const {
	return _bound;
}

const std::vector<std::string>& Request::getMultiBody() const {
	return _multiBody;
}

const std::string&	Request::getType() const {
	return _fileType;
}
//----------------<