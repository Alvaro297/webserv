#include "../inc/Handler.hpp"
#include "../inc/autoindex.hpp"

Handler::Handler(const std::string& root, const ServerConfig& conf) : _root(root), _conf(conf) {}

//Find if the location have autoindex on
bool Handler::isAutoindexEnabled(const std::string& requestPath)
{
	std::vector<LocationConfigStruct> allLocations = this->_conf.getLocations();
	std::string pathLocation;
	size_t		locationPathLength = 0;
	size_t		locationPathMaxLength = 0;
	bool		isAutoindex = this->_conf.getAutoindex();
	for (size_t i = 0; i < allLocations.size(); i++)
	{
		pathLocation = allLocations[i].path;
		if (requestPath.find(pathLocation) == 0)
		{
			locationPathLength = pathLocation.size();
			if (locationPathLength > locationPathMaxLength)
			{
				locationPathMaxLength = locationPathLength;
				isAutoindex = allLocations[i].autoindex;
			}
		}
	}
	return isAutoindex;
}

// Generate the final path of the file. In case the raw only have a '/', need to find the first server default path in the 'index'.
std::string	Handler::buildFilePath(const std::string& rawReq) const {
	if (rawReq == "/") {
		std::vector<std::string> indexV = _conf.getIndex();

		for (size_t i = 0; i < indexV.size(); ++i) {
			std::string ind = _root;
			if (ind[ind.length() - 1] != '/')
				ind += "/";
			ind += indexV[i];

			std::ifstream file(ind.c_str());
			if (file.good())
				return ind;
		}
		return "";
	}
	return _root + rawReq;
}

//Save the content of _multiBody in different files.
bool	saveMultipartFile(std::string part, const std::string& uploadFolder) {
	try {
		std::cout << "[DEBUG] part: " << part << std::endl;
		size_t	dispStart = part.find("Content-Disposition:");

		size_t	nameStart = part.find("name=\"", dispStart);

		if (nameStart == std::string::npos) //RFC says parameter "name=" must exist, but the standard behaviour is accept unnamed parts anyway (perfect multipart reqs aren't usual)
			return true;					// So, we just skip this part

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
		
		std::ofstream out;
		if (uploadFolder.empty())
			out.open(("uploads/" + fileName).c_str(), std::ios::binary); //Open/create a file with that name in uploads directory.
		else
			out.open((uploadFolder + "/" + fileName).c_str(), std::ios::binary); //Open/create a file with that name in uploads directory.
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

		// Determine upload folder using longest location match
		std::string uploadFolder = this->_conf.getUploadStore();
		
		std::string reqPath = req.getPath();
		size_t bestLen = 0;
		const std::vector<LocationConfigStruct>& locs = this->_conf.getLocations();
		for (size_t i = 0; i < locs.size(); ++i) { // If a location-specific upload store exists, use it
			const LocationConfigStruct& loc = locs[i];
			if (!loc.upload_store.empty() && reqPath.compare(0, loc.path.length(), loc.path) == 0 && loc.path.length() > bestLen) {
				bestLen = loc.path.length();
				uploadFolder = loc.upload_store;
			}
		}

		for (size_t i = 0; i < mBody.size(); ++i) {
			if (!saveMultipartFile(mBody[i], uploadFolder))
				throw std::runtime_error("Bad multipart");
		}
		FillResp::set200(res, req, "<h1>File uploaded succesfully</h1>");
		/* if (!success.html)  //Esta parte debe lanzar el HTML de Mario (si existe) cuando lo tenga preparado
			FillResp::set200(res, req, "<h1>File uploaded succesfully</h1>");
		else
			FillResp::set200(res, req, successHtml); */

	}
	catch (const std::exception& e) {
		FillResp::set500(res, req);
		return res;
	}
	return res;
}

// POST Method. Generates the path, generates a writing stream to the file, open the file.
//  Then, writes the _body in the file and closes it.
Response	Handler::handlePOST(Request& req) {
	Response	res;
	try {
		std::string path = buildFilePath(req.getPath()); //Gen path.
		if (path.empty()) {
			FillResp::set404(res, req);
			return res;
		}
		req.setFinalPath(path);

		std::ofstream file(path.c_str(), std::ios::out | std::ios::binary); //Open file in write mode.
		if (!file.is_open())
			throw std::runtime_error("Error opening file");
		file << req.getBody();
		file.close();

		FillResp::set201(res, req);
	}
	catch (const std::exception& e) {
		FillResp::set500(res, req);
		return res;
	}
	return res;
}

// DELETE Method. Check availability of the file: generating its path, ensuring that path exists, is a directory, and is openable.
//  Then, deletes the file.
Response	Handler::handleDELETE(Request& req) {
	Response res;

	try {
		std::string path = buildFilePath(req.getPath()); //Gen path.
		if (path.empty()) {
			FillResp::set404(res, req);
			return res;
		}
		req.setFinalPath(path);

		struct stat s; //Verify path/file availability.
		if (stat(path.c_str(), &s) != 0 || S_ISDIR(s.st_mode)) {
			FillResp::set404(res, req);
			return res;
		}

		if (remove(path.c_str()) != 0) //Removing/deleting file
			throw std::runtime_error("Error deleting file");

		std::string responseMime = "success.html";
		FillResp::set200(res, req, "<h1>200 OK - File Deleted</h1>");
	}
	catch (const std::exception& e) {
		FillResp::set500(res, req);
		return res;
	}
	return res;
}

// GET Method. Check availability of the file: generating its path, ensuring that path exists, is a directory, and is openable.
//  Then, using file content as _body, returns the generated response.
Response	Handler::handleGET(Request& req)
{
	Response res;
	try {
		struct stat s;
		std::string path = buildFilePath(req.getPath());
		if (path.empty())
			path = _root + req.getPath();
		req.setFinalPath(path);
		
		if (stat(path.c_str(), &s) != 0) {
			FillResp::set404(res, req);
			return res;
		}

		if (S_ISDIR(s.st_mode)) {
			if (isAutoindexEnabled(req.getPath())) { // Check if autoindex is enabled for this location
				std::string autoindexHtml = generateAutoindexHtml(path, req.getPath());
				FillResp::set200(res, req, autoindexHtml);
				return res;
			}
			else {
				FillResp::set403(res, req);
				return res;
			}
		}

		std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			FillResp::set403(res, req);
			return res;
		}

		std::ostringstream buff;
		buff << file.rdbuf();
		file.close();

		FillResp::set200(res, req, buff.str());
	}
	catch (const std::exception& e) {
		FillResp::set500(res, req);
	}
	return res;
}

// Check format using parseRequestValidity(), and allow only valid version and methods).
Response	Handler::handleRequest(const std::string& rawReq) {
	Request		req;
	Response	res;
	
	if (!req.parseRequestValidity(rawReq)) {
		FillResp::set400(res, req);
		return res;
	}

	if(req.getVersion() != "HTTP/1.1") {
		FillResp::set505(res, req);
		return res;
	}

	if (!isMethodAllowed(req.getMethod(), req.getPath())){
		FillResp::set405(res, req);
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
		FillResp::set405(res, req);
		return res;
	}
}

// Ensure that the method of the request is allowed by the location in which we are
bool Handler::isMethodAllowed(const std::string& method, const std::string& path) const {
	const LocationConfigStruct* loc = findActualLocation(path);
	if (!loc)
		return true; //If there is no location, we accept every METHOD
	
	const std::vector<std::string>& allowed = loc->methods;
	if (allowed.empty())
		return true; //If the location doesn't have rules, we accept every METHOD

	for (size_t i = 0; i < allowed.size(); ++i) {
		if (allowed[i] == method)
			return true;
	}

	return false;
}

// Find the .conf location in which we are
const LocationConfigStruct* Handler::findActualLocation(const std::string& reqPath) const {
	const std::vector<LocationConfigStruct>& locations = _conf.getLocations(); //Get locations vector

	const LocationConfigStruct* bestMatch = NULL;

	size_t bestLen = 0;

	for (size_t i = 0; i < locations.size(); ++i) { //Check, in every location, for the most similar path
		const std::string& locPath = locations[i].path;
		if (reqPath.find(locPath) == 0 && locPath.size() > bestLen) { //Check, not only for a coincidence but, for the biggest length (the one that has most in common)
			bestMatch = &locations[i];
			bestLen = locPath.size();
		}
	}

	return bestMatch;
}