#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "iostream"
#include <sstream>
#include <map>

class Response
{
	private:
		int			_statusCode;
		std::string	_statusMessage;
		std::map<std::string, std::string>	_headers;
		std::string	_body;

		std::string	genResponseString();

	public:
		Response();

		void setStatus(int code, const std::string& msg);
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& body);

} ;

#endif