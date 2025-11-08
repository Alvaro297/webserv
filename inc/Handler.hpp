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
		
		Response	handleGET(const Request& req);
		Response	handlePOST(const Request& req);
		Response	handleDELETE(const Request& req);
		Response	handleMULT(const Request& req);

		std::string	buildFilePath(const std::string& rawReq) const;
		std::string	getMimeType(const std::string& path) const; //Añadido por Mario

	public:
		Handler(const std::string& root);
		
		Response	handleRequest(const std::string& rawReq);

} ;



#endif
