#include "../inc/configParser.hpp"
#include "../inc/ServerConfig.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <iostream>


std::string ConfigParser::s_lastError;

ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(const ConfigParser& other) { (void)other; }
ConfigParser& ConfigParser::operator=(const ConfigParser& other) { (void)other; return *this; }
ConfigParser::~ConfigParser() {}

const std::string& ConfigParser::LastError() { return s_lastError; }

bool ConfigParser::readAll(const std::string& path, std::string& out) {
	std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);
	if (!ifs) { s_lastError = "Could not open file: " + path; return false; }
	std::ostringstream oss;
	oss << ifs.rdbuf();
	out = oss.str();
	return true;
}

bool ConfigParser::LoadFile(const std::string& path,
							std::vector<ServerConfigStruct>& outServers) {
	s_lastError.clear();
	std::string text;
	if (!readAll(path, text)) return false;
	
	size_t pos = 0;
	int serverCount = 0;
	while ((pos = text.find("server", pos)) != std::string::npos) {
		bool isWholeWord = true;
		if (pos > 0 && std::isalnum(text[pos - 1])) isWholeWord = false;
		if (pos + 6 < text.length() && std::isalnum(text[pos + 6])) isWholeWord = false;
		
		if (isWholeWord) {
			size_t bracePos = pos + 6;
			while (bracePos < text.length() && std::isspace(text[bracePos])) {
				bracePos++;
			}
			if (bracePos < text.length() && text[bracePos] == '{') {
				serverCount++;
				ServerConfigStruct srv;
				srv.port = 8080;
				srv.root = "/var/www/html";
				outServers.push_back(srv);
			}
		}
		pos += 6;
	}
	
	return true;
}

/*bool ConfigParser::RunParser(const char* config_file)
{
	std::vector<ServerConfig> servers;
	if (!LoadFile(std::string(config_file), servers))
	{
		std::cerr << "Error: " << LastError() << std::endl;
		return false;
	}
	//Creo que esto debe retornar o una excepcion o servers que lo necesito para ello
	std::cout << "Found " << servers.size() << " server(s)" << std::endl;
	
	return true;
}*/

std::vector<ServerConfig> ConfigParser::RunParser(const char * config_file)
{
	try
	{
		std::vector<ServerConfigStruct> serversParsing;
		std::vector<ServerConfig> servers;
		LoadFile(std::string(config_file), serversParsing);
		std::cout << "Found " << serversParsing.size() << " server(s)" << std::endl;
		for (size_t i = 0; i < serversParsing.size(); i++)
			servers.push_back(ServerConfig(serversParsing[i], i));
		return servers;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return std::vector<ServerConfig>(); // Return empty vector on error
	}
}