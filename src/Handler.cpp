#include "Handler.hpp"

Handler::Handler(const std::string& root) : _root(root) {}

// Generate the final path of the file.
std::string	Handler::buildFilePath(const std::string& rawReq) const {
	if (rawReq == "/")
		return _root + "/index.html";
	return _root + rawReq;
}

Response	Handler::handlePOST(const Request& req) {
	Response	res;
	try {
		std::string path = buildFilePath(req.getPath()); //Gen path.

		std::ofstream file(path.c_str(), std::ios::out | std::ios::binary); //Open file in write mode.
		if (!file.is_open())
			throw std::runtime_error("Error opening file");
		file << req.getBody();
		file.close();

		res.setStatus(201, "Created");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>201 Created</h1>");
	}
	catch (const std::exception& e) {
		res.setStatus(500, "Internal Server Error");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>500 Internal Server Error</h1>");
	}
	return res;
}

Response	Handler::handleDELETE(const Request& req) {
	Response res;
		try {
		std::string path = buildFilePath(req.getPath()); //Gen path.

		struct stat s; //Verify path/file availability.
		if (stat(path.c_str(), &s) != 0 || S_ISDIR(s.st_mode)) {
			res.setStatus(404, "Not Found");
			res.setHeader("Content-Type", "text/html");
			res.setBody("<h1>404 Not Found</h1>");
			return res;
		}

		if (remove(path.c_str()) != 0) //Removing/deleting file
			throw std::runtime_error("Error deleting file");

		res.setStatus(200, "OK");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>200 OK - File Deleted</h1>");
	}
	catch (const std::exception& e) {
		res.setStatus(500, "Internal Server Error");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>500 Internal Server Error</h1>");
	}
	return res;
}

// Check availability of the file: generating its path, ensuring that path exists, is a directory, and is openable.
//  Then, using file content as _body, returns the generated response.
Response	Handler::handleGET(const Request& req) {
	Response	res;
	try {
		std::string path = buildFilePath(req.getPath()); //Gen path.

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
	}
	catch (const std::exception& e) {
		res.setStatus(500, "Internal Server Error");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>500 Internal Server Error</h1>");
	}
	return res;
}

// Check format using parseRequestValidity(), and allow only valid version and methods).
Response	Handler::handleRequest(const std::string& rawReq) {
	Request		req;
	Response	res;

	if (!req.parseRequestValidity(rawReq)) {
		res.setStatus(400, "Bad Request");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>400 Bad Request</h1>");
		return res;
	}

	if(req.getVersion() != "HTTP/1.1") {
		res.setStatus(505, "HTTP Version Not Supported");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>505 Version Not Supported</h1>");
		return res;
	}

	if (req.getMethod() == "GET")
			return handleGET(req);
	else if (req.getMethod() == "POST")
			return handlePOST(req);
	else if (req.getMethod() == "DELETE")
		return handleDELETE(req);
	else {
		res.setStatus(405, "Method Not Allowed");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>405 Method Not Allowed</h1>");
		return res;
	}
}
