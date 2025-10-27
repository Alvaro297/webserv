#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "Request.hpp"
#include "Response.hpp"

class Handler {
	private:
		std::string	_root;
		
		Response	handleGET(const Request& req);

		std::string	buildFilePath(const std::string& rawReq) const;

	public:
		Handler(const std::string& root);
		
		Response	handleRequest(const std::string& rawReq);

} ;



#endif
