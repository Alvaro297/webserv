#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <map>

class Request
{
	private:
		std::string							_method;
		std::string							_path;
		std::map<std::string, std::string>	_queryMap;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		size_t								_contentLength;

		bool	parsePath();
		bool	parseQuery(std::string rawQuery);
		bool	parseHeaderLine(std::string& line);

	public:
		Request();

		bool	parseRequestValidity(const std::string& rawReq);

		const std::string&	getMethod() const;
		const std::string&	getPath() const;
		const std::string&	getVersion() const;
		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string&	getBody() const;
		const std::map<std::string, std::string>&	getQueryParam(const std::string& key) const;

} ;

#endif