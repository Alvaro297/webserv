#include "../../inc/ServerConfig.hpp"
#include "../../inc/configParser.hpp"
#include <iostream>

ServerConfig::ServerConfig() 
{
	this->id = 0;
	this->port = 8080;
	this->host = "127.0.0.1";
	this->server_name = "localhost";
	this->root = "/var/www/html";
	this->ipv = 4;
}

ServerConfig::ServerConfig(const ServerConfigStruct server, int id)
{
	this->id = id;
	this->port = server.port;
	this->host = normalizeHost(server.host);
	this->server_name = server.server_name;
	this->root = normalizeRoot(server.root);
	this->ipv = server.ipv;
	this->index = server.index;
	this->locations = server.locations;
	this->error_pages = server.error_pages;
	this->client_max_body_size = server.client_max_body_size;
	this->autoindex = server.autoindex;
	this->upload_enable = server.upload_enable;
	this->upload_store = server.upload_store;
	this->cgi_extensions = server.cgi_extensions;
	this->cgi_enable = server.cgi_enable;
}

ServerConfig::ServerConfig(const ServerConfig& other)
{
	if (this != &other)
	{
		this->id = other.id;
		this->port = other.port;
		this->host = other.host;
		this->server_name = other.server_name;
		this->root = other.root;
		this->ipv = other.ipv;
		this->index = other.index;
	}
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other)
{
	if (this != &other)
	{
		this->id = other.id;
		this->port = other.port;
		this->host = other.host;
		this->server_name = other.server_name;
		this->root = other.root;
		this->ipv = other.ipv;
		this->index = other.index;
	}
	return *this;
}

ServerConfig::~ServerConfig() {}


std::string ServerConfig::normalizeRoot(std::string root)
{
	if (root.empty())
		root = "/var/www/html";
	else if (!root.empty() && root[root.length() - 1] == '/')
		root.erase(root.length() - 1);
	return root;
}

std::string ServerConfig::normalizeHost(std::string host)
{
	size_t position = host.find(":"); //Revisar si esto hay que controlarlo o no

	if (position != std::string::npos)
		host = host.substr(0, position); // Aqui se quita el puerto en el host
	host.erase(std::remove(host.begin(), host.end(), ' '), host.end());
	if (host.empty() || host == "*")
		host = "0.0.0.0";
	else if (host == "localhost")
		host = "127.0.0.1";
	return host;
}

// Getters
int ServerConfig::getId() const { return id; }
int ServerConfig::getPort() const { return port; }
const std::string& ServerConfig::getHost() const { return host; }
const std::string& ServerConfig::getServerName() const { return server_name; }
const std::string& ServerConfig::getRoot() const { return root; }
int ServerConfig::getIpv() const { return ipv; }
const std::vector<std::string>& ServerConfig::getIndex() const { return index; }
const std::vector<LocationConfigStruct>& ServerConfig::getLocations() const { return locations; }
const std::map<int, std::string>& ServerConfig::getErrorPages() const { return error_pages; }
size_t ServerConfig::getClientMaxBodySize() const { return client_max_body_size; }
bool ServerConfig::getAutoindex() const { return autoindex; }
bool ServerConfig::getUploadEnable() const { return upload_enable; }
const std::string& ServerConfig::getUploadStore() const { return upload_store; }
const std::map<std::string, std::string>& ServerConfig::getCgiExtensions() const { return cgi_extensions; }
bool ServerConfig::getCgiEnable() const { return cgi_enable; }


// Setters
void ServerConfig::setId(int value) { id = value; }
void ServerConfig::setPort(int value) { port = value; }
void ServerConfig::setHost(const std::string& value) { host = value; }
void ServerConfig::setServerName(const std::string& value) { server_name = value; }
void ServerConfig::setRoot(const std::string& value) { root = value; }
void ServerConfig::setIpv(const int value) { ipv = value; }
void ServerConfig::setIndex(const std::vector<std::string> value) { index = value; }

void ServerConfig::printConfig() const
{
	std::cout << "Server Configuration:" << std::endl;
	std::cout << "  ID: " << id << std::endl;
	std::cout << "  Host: " << host << std::endl;
	std::cout << "  Port: " << port << std::endl;
	std::cout << "  Server Name: " << server_name << std::endl;
	std::cout << "  Root: " << root << std::endl;
	std::cout << "  IPv: " << ipv << std::endl;
	std::cout << "  Index files: ";
	for (size_t i = 0; i < index.size(); ++i) {
		std::cout << index[i];
		if (i < index.size() - 1) std::cout << ", ";
	}
	std::cout << std::endl;

	// Extended options
	std::cout << "  Locations:" << std::endl;
	for (size_t i = 0; i < locations.size(); ++i) {
		const LocationConfigStruct &loc = locations[i];
		std::cout << "    - Path: " << loc.path << std::endl;
		std::cout << "      Methods: ";
		for (size_t m = 0; m < loc.methods.size(); ++m) {
			std::cout << loc.methods[m];
			if (m < loc.methods.size() - 1) std::cout << ", ";
		}
		std::cout << std::endl;
		std::cout << "      Root: " << loc.root << std::endl;
		std::cout << "      Autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
		std::cout << "      Index files: ";
		for (size_t x = 0; x < loc.index.size(); ++x) {
			std::cout << loc.index[x];
			if (x < loc.index.size() - 1) std::cout << ", ";
		}
		std::cout << std::endl;
		std::cout << "      Upload enabled: " << (loc.upload_enable ? "yes" : "no") << std::endl;
		if (loc.upload_enable) std::cout << "      Upload store: " << loc.upload_store << std::endl;
		std::cout << "      CGI enabled: " << (loc.cgi_enable ? "yes" : "no") << std::endl;
		if (!loc.cgi_extensions.empty()) {
			std::cout << "      CGI extensions:" << std::endl;
			for (std::map<std::string, std::string>::const_iterator it = loc.cgi_extensions.begin(); it != loc.cgi_extensions.end(); ++it) {
				std::cout << "        " << it->first << " -> " << it->second << std::endl;
			}
		}
		if (loc.return_code != 0) std::cout << "      Return: " << loc.return_code << " " << loc.return_url << std::endl;
	}

	std::cout << "  Error pages:" << std::endl;
	for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		std::cout << "    " << it->first << " -> " << it->second << std::endl;
	}

	std::cout << "  Client max body size: " << client_max_body_size << std::endl;
	std::cout << "  Autoindex: " << (autoindex ? "on" : "off") << std::endl;
	std::cout << "  Upload enabled: " << (upload_enable ? "yes" : "no") << std::endl;
	if (upload_enable) std::cout << "  Upload store: " << upload_store << std::endl;
	std::cout << "  CGI enabled: " << (cgi_enable ? "yes" : "no") << std::endl;
	if (!cgi_extensions.empty()) {
		std::cout << "  CGI extensions:" << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = cgi_extensions.begin(); it != cgi_extensions.end(); ++it) {
			std::cout << "    " << it->first << " -> " << it->second << std::endl;
		}
	}

	std::cout << std::endl;
}