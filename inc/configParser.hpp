#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>

class ServerConfig;

struct LocationConfigStruct {
	std::string path;                      // location path (e.g. /static)
	std::vector<std::string> methods;      // allowed methods
	std::string root;                      // root for this location
	bool autoindex;
	// Per-location maximum body size in bytes. 0 means not defined (inherit from server)
	size_t client_max_body_size;
	std::vector<std::string> index;
	bool upload_enable;
	std::string upload_store;
	bool cgi_enable;
	std::map<std::string, std::string> cgi_extensions; // ext -> binary
	int return_code;
	std::string return_url;
	LocationConfigStruct(): autoindex(false), client_max_body_size(0), upload_enable(false), cgi_enable(false), return_code(0) {}
};

struct ServerConfigStruct {
	std::string host;
	int			port;
	std::string root;
	std::string server_name;
	int			ipv;
	std::vector<std::string> index;
	std::vector<LocationConfigStruct> locations;
	std::map<int, std::string> error_pages;
	size_t client_max_body_size;
	bool autoindex;
	bool upload_enable;
	std::string upload_store;
	std::map<std::string, std::string> cgi_extensions;
	bool cgi_enable;
	std::vector<std::pair<std::string,int> > listens; // list of host,port pairs
	ServerConfigStruct(): port(8080), ipv(4), client_max_body_size(0), autoindex(false), upload_enable(false), cgi_enable(false) {}
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
	static bool fillLocation(const std::string& block, LocationConfigStruct& location);

public:
	static std::vector<ServerConfig> RunParser(const char* config_file);
};

#endif
