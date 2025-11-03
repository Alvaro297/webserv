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
		else if (key == "listen") {
			// listen may be "host:port" or simply "port"
			std::string listenVal;
			lineStream >> listenVal;
			size_t colon = listenVal.find(":");
			if (colon != std::string::npos) {
				std::string lhost = listenVal.substr(0, colon);
				int lport = std::atoi(listenVal.substr(colon + 1).c_str());
				server.listens.push_back(std::make_pair(lhost, lport));
			} else {
				int lport = std::atoi(listenVal.c_str());
				server.listens.push_back(std::make_pair(std::string("0.0.0.0"), lport));
			}
		}
		else if (key == "root") {
			lineStream >> server.root;
		}
		else if (key == "server_name") {
			lineStream >> server.server_name;
		}
		else if (key == "error_page") {
			int code; std::string path;
			lineStream >> code >> path;
			server.error_pages[code] = path;
		}
		else if (key == "client_max_body_size") {
			std::string sizeStr; lineStream >> sizeStr;
			// support suffixes like 10m, 2k
			size_t mult = 1;
			if (!sizeStr.empty()) {
				char suffix = sizeStr[sizeStr.size()-1];
				if (suffix == 'k' || suffix == 'K') { mult = 1024; sizeStr = sizeStr.substr(0, sizeStr.size()-1); }
				else if (suffix == 'm' || suffix == 'M') { mult = 1024*1024; sizeStr = sizeStr.substr(0, sizeStr.size()-1); }
			}
			server.client_max_body_size = std::atoi(sizeStr.c_str()) * mult;
		}
		else if (key == "autoindex") {
			std::string val; lineStream >> val; server.autoindex = (val == "on" || val == "true");
		}
		else if (key == "upload_store") {
			lineStream >> server.upload_store;
			// Normalize: remove trailing slash (unless it's root "/")
			if (!server.upload_store.empty() && server.upload_store.size() > 1 && server.upload_store[server.upload_store.size() - 1] == '/')
				server.upload_store.erase(server.upload_store.size() - 1);
			// Basic validation: reject traversal
			if (server.upload_store.find("..") != std::string::npos) {
				s_lastError = "Invalid upload_store path (contains '..'): " + server.upload_store;
				return false;
			}
			// If upload_store set, try to create directory (mkdir -p behaviour)
			if (!server.upload_store.empty()) {
				// recursive mkdir
				std::string path = server.upload_store;
				std::string sub;
				for (size_t pos = 0; pos < path.size(); ) {
					size_t next = path.find('/', pos);
					if (next == std::string::npos) next = path.size();
					sub = path.substr(0, next);
					if (sub.empty()) { pos = next + 1; continue; }
					if (mkdir(sub.c_str(), 0755) != 0) {
						if (errno != EEXIST) {
							std::cerr << "Warning: could not create upload_store directory '" << sub << "': " << strerror(errno) << std::endl;
							// don't hard-fail on mkdir; leave path as-is
							break;
						}
					}
					pos = next + 1;
				}
			}
		}
		else if (key == "upload_enable") {
			std::string val; lineStream >> val; server.upload_enable = (val == "on" || val == "true");
		}
		else if (key == "cgi_extension") {
			std::string ext, bin;
			lineStream >> ext >> bin;
			server.cgi_extensions[ext] = bin;
		}
		else if (key == "cgi_enable") {
			std::string val; lineStream >> val; server.cgi_enable = (val == "on" || val == "true");
		}
		else if (key == "index") {
			server.index.clear();
			std::string indexFile;
			while (lineStream >> indexFile) {
				server.index.push_back(indexFile);
			}
		}
		else if (key == "location") {
			// read until '{' to find path, then extract inner block until matching '}'
			std::string path;
			lineStream >> path;
			// We need to find the block start position in the original block string. Simplest approach: find "location <path>" and then extract between braces.
			std::string marker = "location ";
			size_t pos = block.find(marker + path);
			if (pos != std::string::npos) {
				size_t bracePos = block.find('{', pos);
				if (bracePos != std::string::npos) {
					size_t innerStart = bracePos + 1;
					size_t innerEnd = innerStart;
					int braceCount = 1;
					while (innerEnd < block.length() && braceCount > 0) {
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



// Definition added to match declaration in header
bool ConfigParser::fillLocation(const std::string& block, LocationConfigStruct& location) {
	std::istringstream iss(block);
	std::string line;
	while (std::getline(iss, line)) {
		size_t start = line.find_first_not_of(" \t\r\n");
		if (start == std::string::npos) continue;
		size_t end = line.find_last_not_of(" \t\r\n");
		line = line.substr(start, end - start + 1);
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
				std::string path = location.upload_store;
				std::string sub;
				for (size_t pos = 0; pos < path.size(); ) {
					size_t next = path.find('/', pos);
					if (next == std::string::npos) next = path.size();
					sub = path.substr(0, next);
					if (sub.empty()) { pos = next + 1; continue; }
					if (mkdir(sub.c_str(), 0755) != 0) {
						if (errno != EEXIST) {
							std::cerr << "Warning: could not create upload_store directory '" << sub << "': " << strerror(errno) << std::endl;
							break;
						}
					}
					pos = next + 1;
				}
			}
		} else if (key == "cgi_extension") {
			std::string ext, bin; ls >> ext >> bin; if (!ext.empty()) location.cgi_extensions[ext] = bin;
		} else if (key == "cgi_enable") {
			std::string val; ls >> val; location.cgi_enable = (val == "on" || val == "true");
		} else if (key == "return") {
			int code = 0; std::string url; ls >> code >> url; location.return_code = code; location.return_url = url;
		} else {
			// Unknown directive: ignore for now
		}
	}
	return true;
}


