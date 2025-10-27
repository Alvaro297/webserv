#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <map>

class Request
{
	private:
		std::string	_method;
		std::string	_path;
		std::string	_version;
		std::map<std::string, std::string>	_headers;
		std::string	_body;
		size_t		_contentLength;

		bool	parseHeaderLine(std::string line);

	public:
		Request();

		bool	parseRequestValidity(const std::string& rawReq);

		const std::string&	getMethod() const;
		const std::string&	getPath() const;
		const std::string&	getVersion() const;
		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string&	getBody() const;

} ;

#endif