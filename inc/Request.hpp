#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

class Request
{
	private:
		std::string							_method;
		std::string							_path;
		std::string							_finalPath;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::string							_body;

		size_t								_contentLength;
		std::string							_bound;
		std::vector<std::string>			_multiBody;
		std::map<std::string, std::string>	_queryMap;
		std::string							_fileType;
		
		bool	parsePath();
		bool	parseQuery(std::string rawQuery);
		bool	parseHeaderLine(std::string& line);
		bool	parseMultipart(std::string& multi);
		bool	findBoundary(std::string& multi);

		void	setFileType();
		
		public:
		Request();

		void	setFinalPath(const std::string& path);
		static std::string	getMimeType(const std::string&);
		bool	parseRequestValidity(const std::string& rawReq);

		const std::string&	getMethod() const;
		const std::string&	getPath() const;
		const std::string&	getFinalPath() const;
		const std::string&	getVersion() const;
		const std::string&	getType() const;
		const std::map<std::string, std::string>&	getHeaders() const;
		const std::string&	getBody() const;
		const std::string&	getQueryParam(const std::string& key) const;
		const std::string&  getBound() const;
		const std::vector<std::string>& getMultiBody() const;

} ;

#endif