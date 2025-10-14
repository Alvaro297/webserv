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

	static bool readAll(const std::string& path, std::string& out);
	static const std::string& LastError();
	static bool LoadFile(const std::string& path,
						 std::vector<ServerConfigStruct>& outServers);

public:
//    static bool RunParser(const char* config_file);
	static std::vector<ServerConfig> RunParser(const char* config_file);
};

#endif
