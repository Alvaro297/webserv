#include "ServerConfig.hpp"


ServerConfig::ServerConfig(const ServerConfigStruct server)
{
	
}

// Getters
int ServerConfig::getId() const { return id; }
int ServerConfig::getPort() const { return port; }
const std::string& ServerConfig::getHost() const { return host; }
const std::string& ServerConfig::getServerName() const { return server_name; }
const std::string& ServerConfig::getRoot() const { return root; }
int ServerConfig::getIpv() const { return ipv; }
const std::vector<std::string>& ServerConfig::getIndex() const { return index; }


// Setters
void ServerConfig::setId(int value) { id = value; }
void ServerConfig::setPort(int value) { port = value; }
void ServerConfig::setHost(const std::string& value) { host = value; }
void ServerConfig::setServerName(const std::string& value) { server_name = value; }
void ServerConfig::setRoot(const std::string& value) { root = value; }
void ServerConfig::setIpv(const int value) { ipv = value; }
void ServerConfig::setIndex(const std::vector<std::string> value) { index = value; }