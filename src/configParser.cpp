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

bool ConfigParser::readFile(const std::string& path, std::string& out) {
	std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);
	if (!ifs) { s_lastError = "Could not open file: " + path; return false; }
	std::ostringstream oss;
	oss << ifs.rdbuf();
	out = oss.str();
	return true;
}


bool ConfigParser::fillServer(const std::string& block, ServerConfigStruct& server) {
	std::istringstream iss(block);
	std::string line;
	
	while (std::getline(iss, line)) {
		size_t start = line.find_first_not_of(" \t\r\n");
		if (start == std::string::npos) continue;
		
		size_t end = line.find_last_not_of(" \t\r\n");
		line = line.substr(start, end - start + 1);
		
		if (line.empty() || line[0] == '#') continue;
		
		if (!line.empty() && line[line.length() - 1] == ';') {
			line.erase(line.length() - 1);
		}
		
		std::istringstream lineStream(line);
		std::string key;
		lineStream >> key;
		
		if (key == "host") {
			lineStream >> server.host;
		}
		else if (key == "port") {
			lineStream >> server.port;
		}
		else if (key == "root") {
			lineStream >> server.root;
		}
		else if (key == "server_name") {
			lineStream >> server.server_name;
		}
		else if (key == "index") {
			server.index.clear();
			std::string indexFile;
			while (lineStream >> indexFile) {
				server.index.push_back(indexFile);
			}
		}
	}
	
	return true;
}


bool ConfigParser::searchServers(const std::string& text,
							std::vector<ServerConfigStruct>& outServers) {
	
	size_t pos = 0;
	while ((pos = text.find("server", pos)) != std::string::npos) {

		// Check if 'server' is not part of another word
		if (pos > 0 && text[pos - 1] != '\n'){
			pos += 6;
			continue;
		} 

		// Search for opening brace '{'
		size_t bracePos = pos + 6;
		while (bracePos < text.length() && std::isspace(text[bracePos])) {
			bracePos++;
		}

		if (bracePos < text.length() && text[bracePos] == '{') {
			size_t blockStart = bracePos + 1;
			size_t blockEnd = blockStart;
			int braceCount = 1;
			
			// Find the matching closing brace '}'
			while (blockEnd < text.length() && braceCount > 0) {
				if (text[blockEnd] == '{') braceCount++;
				else if (text[blockEnd] == '}') braceCount--;
				blockEnd++;
			}
			
			if (braceCount == 0) {
				std::string serverBlock = text.substr(blockStart, blockEnd - blockStart - 1);
				ServerConfigStruct srv;
				srv.port = 8080;
				srv.host = "127.0.0.1";
				srv.root = "/var/www/html";
				srv.server_name = "";
				srv.ipv = 4;
				
				if (fillServer(serverBlock, srv)) {
					outServers.push_back(srv);
				}
			}
			else {
				s_lastError = "Mismatched braces in server block.";
				return false;
			}
		}
		pos += 6;
	}
	return true;
}

std::vector<ServerConfig> ConfigParser::RunParser(const char * config_file)
{
	try
	{
		std::vector<ServerConfigStruct> serversParsing;
		std::vector<ServerConfig> servers;
		std::string text;

		if (!readFile((config_file), text)){
			std::cerr << "Error: " << LastError() << std::endl;
			return std::vector<ServerConfig>(); // Return empty vector on error
		}
		if (!searchServers(text, serversParsing)) {
			std::cerr << "Error: " << LastError() << std::endl;
			return std::vector<ServerConfig>(); // Return empty vector on error
		}
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


