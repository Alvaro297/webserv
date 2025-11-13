#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "Request.hpp"
#include "Response.hpp"
#include "FillResp.hpp"
#include "ServerConfig.hpp"


class Handler {
	private:
		std::string		_root;
		ServerConfig	_conf;
		
		Response	handleGET(Request& req);
		Response	handlePOST(Request& req);
		Response	handleDELETE(Request& req);
		Response	handleMULT(const Request& req);

		std::string	buildFilePath(const std::string& rawReq) const;
		std::string	getMimeType(const std::string& path) const; //Añadido por Mario
		bool		isAutoindexEnabled(const std::string& requestPath);
		bool Handler::isMethodAllowed(const std::string& method, const std::string& path) const;
		const LocationConfigStruct* Handler::findActualLocation(const std::string& path) const;

	public:
		Handler(const std::string& root, const ServerConfig& conf);
		
		Response	handleRequest(const std::string& rawReq);

} ;



#endif
