#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>

class ServerConfig;


struct ServerConfigStruct {
	std::string host;
	int			port;
	std::string root;
	std::string server_name;
	int			ipv;
	std::vector<std::string> index;
};

class ConfigParser {
private:
	static std::string s_lastError;

	ConfigParser();
	ConfigParser(const ConfigParser& other);
	ConfigParser& operator=(const ConfigParser& other);
	~ConfigParser();

	static bool readFile(const std::string& path, std::string& out);
	static const std::string& LastError();
	static bool searchServers(const std::string& text,
							std::vector<ServerConfigStruct>& outServers);
	static bool fillServer(const std::string& block, ServerConfigStruct& server);

public:
	static std::vector<ServerConfig> RunParser(const char* config_file);
};

#endif
