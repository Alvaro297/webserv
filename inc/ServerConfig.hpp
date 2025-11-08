#ifndef ServerConfig_HPP
# define ServerConfig_HPP

# include "configParser.hpp"
# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <algorithm>
# include <sys/socket.h>

struct ServerConfigStruct;
struct LocationConfigStruct;


class ServerConfig
{
private:
	int							id;
	int							port;
	std::string					host;
	std::string					server_name;
	std::string					root;
	int							ipv;
	std::vector<std::string>	index;

	// Extended options
	std::vector<LocationConfigStruct> locations;
	std::map<int, std::string> error_pages;
	size_t client_max_body_size;
	bool autoindex;
	bool upload_enable;
	std::string upload_store;
	std::map<std::string, std::string> cgi_extensions;
	bool cgi_enable;


	std::string normalizeRoot(std::string root);
	std::string normalizeHost(std::string host);
public:
	ServerConfig();
	ServerConfig(const ServerConfigStruct server, int i);
	ServerConfig(const ServerConfigStruct server, int i, bool fromParser);
	ServerConfig(const ServerConfig& other);
	ServerConfig& operator=(const ServerConfig& other);
	~ServerConfig();

	void printConfig() const;

	//Getters
	int getId() const;
	int getPort() const;
	const std::string& getHost() const;
	const std::string& getServerName() const;
	const std::string& getRoot() const;
	int getIpv() const;
	const std::vector<std::string>& getIndex() const;
	const std::vector<LocationConfigStruct>& getLocations() const;
	const std::map<int, std::string>& getErrorPages() const;
	size_t getClientMaxBodySize() const;
	bool getAutoindex() const;
	bool getUploadEnable() const;
	const std::string& getUploadStore() const;
	const std::map<std::string, std::string>& getCgiExtensions() const;
	bool getCgiEnable() const;

	//Setters
	void setId(int value);
	void setPort(int value);
	void setHost(const std::string& value);
	void setServerName(const std::string& value);
	void setRoot(const std::string& value);
	void setIpv(const int ipv);
	void setIndex(const std::vector<std::string> index);
	void setLocations(const std::vector<LocationConfigStruct>& locs);
	void setErrorPages(const std::map<int, std::string>& pages);
	void setClientMaxBodySize(const size_t size);
	void setAutoindex(const bool value);
	void setUploadEnable(const bool value);
	void setUploadStore(const std::string& store);
	void setCgiExtensions(const std::map<std::string, std::string>& exts);
	void setCgiEnable(const bool value);

	bool supportsExtension(const std::string& ext) const;
};



#endif