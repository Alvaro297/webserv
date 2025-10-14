#include "../../inc/ServerConfig.hpp"
#include "../../inc/configParser.hpp"

ServerConfig::ServerConfig(const ServerConfigStruct server, int id)
{
	this->id = id;
	this->port = server.port;
	this->host = normalizeHost(server.host);
	this->server_name = server.server_name;
	this->root = normalizeRoot(server.root);
	this->ipv = server.ipv;
	this->index = server.index;
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


// Setters
void ServerConfig::setId(int value) { id = value; }
void ServerConfig::setPort(int value) { port = value; }
void ServerConfig::setHost(const std::string& value) { host = value; }
void ServerConfig::setServerName(const std::string& value) { server_name = value; }
void ServerConfig::setRoot(const std::string& value) { root = value; }
void ServerConfig::setIpv(const int value) { ipv = value; }
void ServerConfig::setIndex(const std::vector<std::string> value) { index = value; }