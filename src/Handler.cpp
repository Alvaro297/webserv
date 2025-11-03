#include "../inc/Handler.hpp"

Handler::Handler(const std::string& root) : _root(root) {}

static std::string getMimeTypeFromPath(const std::string& path) {
	size_t pos = path.rfind('.');
	if (pos == std::string::npos) return "application/octet-stream";
	std::string ext = path.substr(pos);
	if (ext == ".html" || ext == ".htm") return "text/html";
	if (ext == ".css") return "text/css";
	if (ext == ".js") return "application/javascript";
	if (ext == ".json") return "application/json";
	if (ext == ".png") return "image/png";
	if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
	if (ext == ".gif") return "image/gif";
	if (ext == ".svg") return "image/svg+xml";
	if (ext == ".txt") return "text/plain";
	if (ext == ".xml") return "application/xml";
	return "application/octet-stream";
} //MARIO

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

		if (nameStart == std::string::npos) //There is no "name"
			return false; //RFC says parameter "name=" must exist, so I consider this as an error

		size_t	nameEnd = part.find("\"", nameStart + 6);
		std::string	name =part.substr(nameStart + 6, nameEnd - (nameStart + 6));
		
		size_t	contStart = part.find("\r\n\r\n");
		if (contStart == std::string::npos) //No content, so must be a format error
			return false;
		contStart += 4; //Content starts after double jump

		size_t	filenameStart = part.find("filename=\"", dispStart);
		
		if (filenameStart == std::string::npos) { //Not a file, i don't think we need to handle this kind of forms (without archive)
			/* std::string nameValue = part.substr(contStart);
			if (nameValue.size() >= 2 && nameValue[nameValue.size() - 2] == '\r')
				nameValue.erase(nameValue.size() - 2); */
			return true;
		}

		size_t	filenameEnd = part.find("\"", filenameStart + 10);
		std::string	fileName = part.substr(filenameStart + 10, filenameEnd - (filenameStart + 10));
		
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
				throw std::runtime_error("Bad multipart");
		}
		res.setStatus(200, "OK");
		res.setHeader("Content-Type", req.getType());
		res.setBody("<h1>File uploaded succesfully</h1>");
	}
	catch (const std::exception& e) {
		res.setStatus(500, "Internal Server Error");
		res.setHeader("Content-Type", req.getType());
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
		res.setHeader("Content-Type", req.getType());
		res.setBody("<h1>201 Created</h1>");
	}
	catch (const std::exception& e) {
		res.setStatus(500, "Internal Server Error");
		res.setHeader("Content-Type", req.getType());
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
			res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>404 Not Found</h1>");
			return res;
		}

		if (remove(path.c_str()) != 0) //Removing/deleting file
			throw std::runtime_error("Error deleting file");

		res.setStatus(200, "OK");
		res.setHeader("Content-Type", req.getType());
		res.setBody("<h1>200 OK - File Deleted</h1>");
	}
	catch (const std::exception& e) {
		res.setStatus(500, "Internal Server Error");
		res.setHeader("Content-Type", req.getType());
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
		res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>404 Not Found</h1>");
			return res;
		}

		std::ifstream file(path.c_str(), std::ios::in | std::ios::binary); //Open file in read mode.
		if (!file.is_open()) {
			res.setStatus(403, "Forbidden");
		res.setHeader("Content-Type", req.getType());
			res.setBody("<h1>403 Forbidden</h1>");
			return res;
		}

		std::ostringstream buff; //Read file (sending content to buff).
		buff << file.rdbuf();
		file.close();

		res.setStatus(200, "OK");
		res.setHeader("Content-Type", getMimeTypeFromPath(path)); //Mario
		res.setBody(buff.str());
	}
	catch (const std::exception& e) {
		res.setStatus(500, "Internal Server Error");
		res.setHeader("Content-Type", req.getType());
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
		res.setHeader("Content-Type", req.getType());
		res.setBody("<h1>400 Bad Request</h1>");
		return res;
	}

	if(req.getVersion() != "HTTP/1.1") {
		res.setStatus(505, "HTTP Version Not Supported");
		res.setHeader("Content-Type", req.getType());
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
		res.setHeader("Content-Type", req.getType());
		res.setBody("<h1>405 Method Not Allowed</h1>");
		return res;
	}
}
