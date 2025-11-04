#include "../inc/configParser.hpp"
#include "../inc/ServerConfig.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <cstring>

ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(const ConfigParser& other) { (void)other; }
ConfigParser& ConfigParser::operator=(const ConfigParser& other) { (void)other; return *this; }
ConfigParser::~ConfigParser() {}

std::string ConfigParser::s_lastError;
const std::string& ConfigParser::LastError() { return s_lastError; }

// Helper: trim spaces from both ends
static std::string trim_str(const std::string& s) {
	size_t a = s.find_first_not_of(" \t\r\n");
	if (a == std::string::npos) return std::string();
	size_t b = s.find_last_not_of(" \t\r\n");
	return s.substr(a, b - a + 1);
}

// Helper: recursive mkdir -p style (silently ignore EEXIST)
static bool make_dirs(const std::string& path) {
	if (path.empty()) return true;
	std::string sub;
	for (size_t pos = 0; pos < path.size(); ) {
		size_t next = path.find('/', pos);
		if (next == std::string::npos) next = path.size();
		sub = path.substr(0, next);
		if (sub.empty()) { pos = next + 1; continue; }
		if (mkdir(sub.c_str(), 0755) != 0) {
			if (errno != EEXIST) {
				std::cerr << "Warning: could not create directory '" << sub << "': " << strerror(errno) << std::endl;
				return false;
			}
		}
		pos = next + 1;
	}
	return true;
}


bool ConfigParser::readFile(const std::string& path, std::string& out) {
	std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);
	if (!ifs) { s_lastError = "Could not open file: " + path; return false; }
	std::ostringstream oss;
	oss << ifs.rdbuf();
	out = oss.str();
	return true;
}

// Definition added to match declaration in header
bool ConfigParser::fillLocation(const std::string& block, LocationConfigStruct& location) {
	std::istringstream iss(block);
	std::string line;
	while (std::getline(iss, line)) {
		line = trim_str(line);
		if (line.empty() || line[0] == '#') continue;
		if (!line.empty() && line[line.length() - 1] == ';') line.erase(line.size() - 1);

		std::istringstream ls(line);
		std::string key;
		ls >> key;

		if (key == "methods") {
			location.methods.clear();
			std::string m;
			while (ls >> m) location.methods.push_back(m);
		} else if (key == "root") {
			ls >> location.root;
		} else if (key == "autoindex") {
			std::string val; ls >> val; location.autoindex = (val == "on" || val == "true");
		} else if (key == "index") {
			location.index.clear(); std::string idx; while (ls >> idx) location.index.push_back(idx);
		} else if (key == "upload_enable") {
			std::string val; ls >> val; location.upload_enable = (val == "on" || val == "true");
		} else if (key == "upload_store") {
			ls >> location.upload_store;
			// Normalize remove trailing slash
			if (!location.upload_store.empty() && location.upload_store.size() > 1 && location.upload_store[location.upload_store.size() - 1] == '/')
				location.upload_store.erase(location.upload_store.size() - 1);
			if (location.upload_store.find("..") != std::string::npos) {
				s_lastError = "Invalid upload_store path in location (contains '..'): " + location.upload_store;
				return false;
			}
			if (!location.upload_store.empty()) {
				make_dirs(location.upload_store);
			}
		} else if (key == "cgi_extension") {
			std::string ext, bin; ls >> ext >> bin; if (!ext.empty()) location.cgi_extensions[ext] = bin;
		} else if (key == "cgi_enable") {
			std::string val; ls >> val; location.cgi_enable = (val == "on" || val == "true");
		} else if (key == "return") {
			int code = 0; std::string url; ls >> code >> url; location.return_code = code; location.return_url = url;
		}
	}
	return true;
}

bool ConfigParser::fillServer(const std::string& block, ServerConfigStruct& server) {
	std::istringstream iss(block);
	std::string line;
	// position to continue searching for location markers inside this block
	size_t searchPos = 0;

	while (std::getline(iss, line)) {
		line = trim_str(line);
		if (line.empty() || line[0] == '#')
			continue;

		// Quitar punto y coma final si existe
		if (!line.empty() && line[line.size() - 1] == ';')
			line.erase(line.size() - 1, 1);

		std::istringstream lineStream(line);
		std::string key;
		lineStream >> key;

		// --- Claves simples ---
		if (key == "root") lineStream >> server.root;
		else if (key == "server_name") lineStream >> server.server_name;
		else if (key == "host") lineStream >> server.host;
		else if (key == "port") lineStream >> server.port;
		else if (key == "client_max_body_size") lineStream >> server.client_max_body_size;
		else if (key == "index") {
			server.index.clear();
			std::string file;
			while (lineStream >> file)
				server.index.push_back(file);
		}
		else if (key == "error_page") {
			int code;
			std::string path;
			lineStream >> code >> path;
			if (!path.empty())
				server.error_pages[code] = path;
		}
		// --- Clave location ---
		else if (key == "location") {
			std::string path;
			lineStream >> path;

			std::string marker = "location " + path;
			size_t pos = block.find(marker, searchPos);
			if (pos != std::string::npos) {
				size_t bracePos = block.find('{', pos);
				if (bracePos != std::string::npos) {
					size_t innerStart = bracePos + 1;
					size_t innerEnd = innerStart;
					int braceCount = 1;

					while (innerEnd < block.size() && braceCount > 0) {
						if (block[innerEnd] == '{') braceCount++;
						else if (block[innerEnd] == '}') braceCount--;
						innerEnd++;
					}

					if (braceCount == 0) {
						std::string inner = block.substr(innerStart, innerEnd - innerStart - 1);
						LocationConfigStruct loc;
						loc.path = path;

						if (!fillLocation(inner, loc)) {
							s_lastError = "Error parsing location: " + path;
							return false;
						}
						server.locations.push_back(loc);
						searchPos = innerEnd;
					}
				}
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
				if (fillServer(serverBlock, srv))
					outServers.push_back(srv);
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
