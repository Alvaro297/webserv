#include "../inc/Handler.hpp"

Handler::Handler(const std::string& root) : _root(root) {}

// Generate the final path of the file.
std::string	Handler::buildFilePath(const std::string& rawReq) const {
	if (rawReq == "/")
		return _root + "/index.html";
	return _root + rawReq;
}

//Save the content of _multiBody in different files.
bool	saveMultipartFile(std::string part) {
	try {
		size_t	dispStart = part.find("Content-Disposition:");
		size_t	nameStart = part.find("name=\"", dispStart);
		size_t	filenameStart = part.find("filename=\"", dispStart);
		
		if (filenameStart == std::string::npos) //Not a file, so nothing to do
		return ;
		
		size_t	filenameEnd = part.find("\"", filenameStart + 10);
		std::string	fileName = part.substr(filenameStart + 10, filenameEnd - (filenameStart + 10));
		
		size_t	contStart = part.find("\r\n\r\n");
		if (contStart == std::string::npos) //No content, so nothing to do
		return ;
		contStart += 4; //Content starts after double jump
		
		std::string	content = part.substr(contStart); //String with all the content
		if (content.size() >= 2 && content[content.size() - 2] == '\r')
		content.erase(content.size() - 2); //Remove last "\r\n"
		
		
		std::ofstream out(("uploads/" + fileName).c_str(), std::ios::binary); //Open/create a file with that name in uploads directory.
		out.write(content.c_str(), content.size()); //Use write instead of <<, because binaries can have a "\0" inside the text.
		out.close();
	}
	catch (const std::exception& e) {
		return false;
	}

	return true;
}

// This is NOT a real method. Is an internal adaptation to handle multipart bodies.
Response Handler::handleMULT(const Request& req) {
	Response res;

	try {
		std::vector<std::string> mBody = req.getMultiBody();

		for (size_t i = 0; i < mBody.size(); ++i) {
			if (!saveMultipartFile(mBody[i]))
				throw ;
		}
		res.setStatus(200, "OK");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>File uploaded succesfully</h1>");
	}
	catch (const std::exception& e) {
		res.setStatus(500, "Internal Server Error");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>500 Internal Server Error</h1>");
	}
	return res;
}

// POST Method. Generates the path, generates a writing stream to the file, open the file.
//  Then, writes the _body in the file and closes it.
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

// DELETE Method. Check availability of the file: generating its path, ensuring that path exists, is a directory, and is openable.
//  Then, deletes the file.
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

// GET Method. Check availability of the file: generating its path, ensuring that path exists, is a directory, and is openable.
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
		res.setHeader("Content-Type", getMimeType(path)); //añadido por Mario
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
	else if (req.getMethod() == "POST") {
		if (!(req.getBound()).empty()) //POST can have _multiBody, so if _bound is setted, use handleMULT() instead of handlePOST()
			return handleMULT(req);
		return handlePOST(req);
	}
	else if (req.getMethod() == "DELETE")
		return handleDELETE(req);
	else {
		res.setStatus(405, "Method Not Allowed");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<h1>405 Method Not Allowed</h1>");
		return res;
	}
}
