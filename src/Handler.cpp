#include "Handler.hpp"

Handler::Handler(const std::string& root) : _root(root) {}

// Generate the final path of the file.
std::string	Handler::buildFilePath(const std::string& rawReq) const {
	if (rawReq == "/")
		return _root + "/index.html";
	return _root + rawReq;
}

// Check availability of the file: generating its path, ensuring that path exists, is a directory, and is openable.
//  Then, using file content as _body, returns the generated response.
Response	Handler::handleGET(const Request& req) {
	std::string path = buildFilePath(req.getPath()); //Gen path.
	Response	res;

	struct stat s; //Verify path/file availability.
	if (stat(path.c_str(), &s) != 0 || S_ISDIR(s.st_mode)) {
		res.setStatus(404, "Not Found");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>404 Not Found</h1>");
		return res;
	}

	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary); //Open file in read mode.
	if (!file.is_open()) {
		res.setStatus(403, "Forbidden");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>403 Forbidden</h1>");
		return res;
	}

	std::ostringstream buff; //Read file (sending content to buff).
	buff << file.rdbuf();
	file.close();

	res.setStatus(200, "OK");
	res.setHeader("Content-Type", "text/html");
	res.setBody(buff.str());
	return res;
}

// Check format: using parseRequestValidity(), and allowing only GET method).
Response	Handler::handleRequest(const std::string& rawReq) {
	Request		req;
	Response	res;

	if (req.parseRequestValidity(rawReq)) {
		if (req.getMethod() == "GET")
			res = handleGET(req);
		else {
			res.setStatus(405, "Method Not Allowed");
			res.setHeader("Content-Type", "text/html");
			res.setBody("<h1>405 Method Not Allowed</h1>");
		}
	}
	else {
		res.setStatus(400, "Bad Request");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>400 Bad Request</h1>");
	}

	return res;
}
