#include "../inc/configParser.hpp"
#include "../inc/ServerConfig.hpp"

ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(const ConfigParser& other) { (void)other; }
ConfigParser& ConfigParser::operator=(const ConfigParser& other) { (void)other; return *this; }
ConfigParser::~ConfigParser() {}

std::string ConfigParser::s_lastError;
const std::string& ConfigParser::LastError() { return s_lastError; }

// Trim whitespace from both ends of string
static std::string trim_str(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Parse size strings: "10M", "1K", "2G" or plain bytes
static size_t parse_size(const std::string& s) {
    if (s.empty()) return 0;
    
    std::string t = s;
    if (!t.empty() && t[t.size() - 1] == ';') 
        t.resize(t.size() - 1);
    
    // Detect suffix multiplier
    char suffix = t[t.size() - 1];
    size_t multiplier = 1;
    if (suffix == 'K' || suffix == 'k') { 
        multiplier = 1024ULL; 
        t.resize(t.size() - 1); 
    } else if (suffix == 'M' || suffix == 'm') { 
        multiplier = 1024ULL * 1024ULL; 
        t.resize(t.size() - 1); 
    } else if (suffix == 'G' || suffix == 'g') { 
        multiplier = 1024ULL * 1024ULL * 1024ULL; 
        t.resize(t.size() - 1); 
    }

    if (t.empty()) return 0;
    char *endptr = NULL;
    unsigned long long val = strtoull(t.c_str(), &endptr, 10);
    if (endptr == t.c_str()) return 0;
    return static_cast<size_t>(val * multiplier);
}

// Create directories recursively (like mkdir -p)
static bool make_dirs(const std::string& path) {
    if (path.empty()) return true;
    
    std::string sub;
    for (size_t pos = 0; pos < path.size(); ) {
        size_t next = path.find('/', pos);
        if (next == std::string::npos) next = path.size();
        sub = path.substr(0, next);
        
        if (sub.empty()) { 
            pos = next + 1; 
            continue; 
        }
        
        if (mkdir(sub.c_str(), 0755) != 0 && errno != EEXIST) {
            std::cerr << "Warning: could not create directory '" << sub 
                      << "': " << strerror(errno) << std::endl;
            return false;
        }
        pos = next + 1;
    }
    return true;
}

// Read entire file into a string
bool ConfigParser::readFile(const std::string& path, std::string& out) {
    std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);
    if (!ifs) { 
        s_lastError = "Could not open file: " + path; 
        return false; 
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    out = oss.str();
    return true;
}

// Parse location block configuration
bool ConfigParser::fillLocation(const std::string& block, LocationConfigStruct& location) {
    std::istringstream iss(block);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim_str(line);
        if (line.empty() || line[0] == '#') continue;
        if (!line.empty() && line[line.length() - 1] == ';') 
            line.resize(line.size() - 1);

        std::istringstream ls(line);
        std::string key;
        ls >> key;

        // Parse directives
        if (key == "methods") {
            location.methods.clear();
            std::string m;
            while (ls >> m) location.methods.push_back(m);
        } else if (key == "client_max_body_size") {
            std::string val; 
            ls >> val; 
            location.client_max_body_size = parse_size(val);
        } else if (key == "root") {
            ls >> location.root;
        } else if (key == "autoindex") {
            std::string val; 
            ls >> val; 
            location.autoindex = (val == "on" || val == "true");
        } else if (key == "index") {
            location.index.clear(); 
            std::string idx; 
            while (ls >> idx) location.index.push_back(idx);
        } else if (key == "upload_enable") {
            std::string val; 
            ls >> val; 
            location.upload_enable = (val == "on" || val == "true");
        } else if (key == "upload_store") {
            ls >> location.upload_store;
            
            // Remove trailing slash
            if (!location.upload_store.empty() && 
                location.upload_store[location.upload_store.size() - 1] == '/')
                location.upload_store.resize(location.upload_store.size() - 1);
            
            // Security check: reject paths with '..'
            if (location.upload_store.find("..") != std::string::npos) {
                s_lastError = "Invalid upload_store path (contains '..'): " + location.upload_store;
                return false;
            }
            
            if (!location.upload_store.empty())
                make_dirs(location.upload_store);
        } else if (key == "cgi_extension") {
            std::string ext, bin; 
            ls >> ext >> bin; 
            if (!ext.empty()) location.cgi_extensions[ext] = bin;
        } else if (key == "cgi_ext") {
            std::vector<std::string> tokens;
            std::string tkn;
            while (ls >> tkn) tokens.push_back(tkn);
            
            for (size_t i = 0; i < tokens.size(); ) {
                std::string ext = tokens[i];
                if (i + 1 < tokens.size() && !tokens[i+1].empty() && tokens[i+1][0] == '/') {
                    location.cgi_extensions[ext] = tokens[i+1];
                    i += 2;
                } else {
                    location.cgi_extensions[ext] = "";
                    i += 1;
                }
            }
        } else if (key == "cgi_enable") {
            std::string val; 
            ls >> val; 
            location.cgi_enable = (val == "on" || val == "true");
        } else if (key == "return") {
            int code = 0; 
            std::string url; 
            ls >> code >> url; 
            location.return_code = code; 
            location.return_url = url;
        }
    }
    return true;
}

// Parse server block configuration
bool ConfigParser::fillServer(const std::string& block, ServerConfigStruct& server) {
    std::istringstream iss(block);
    std::string line;
    size_t searchPos = 0;

    while (std::getline(iss, line)) {
        line = trim_str(line);
        if (line.empty() || line[0] == '#') continue;
        if (line[line.size() - 1] == ';') line.resize(line.size() - 1);

        std::istringstream lineStream(line);
        std::string key;
        lineStream >> key;

        // Simple key-value directives
        if (key == "root") {
            lineStream >> server.root;
        } else if (key == "server_name") {
            lineStream >> server.server_name;
        } else if (key == "host") {
            lineStream >> server.host;
        } else if (key == "port") {
            lineStream >> server.port;
        } else if (key == "client_max_body_size") {
            std::string val; 
            lineStream >> val; 
            server.client_max_body_size = parse_size(val);
        } else if (key == "index") {
            server.index.clear();
            std::string file;
            while (lineStream >> file) server.index.push_back(file);
        } else if (key == "error_page") {
            int code;
            std::string path;
            lineStream >> code >> path;
            if (!path.empty()) server.error_pages[code] = path;
        } else if (key == "location") {
            std::string path;
            lineStream >> path;

            // Find location block in text
            std::string marker = "location " + path;
            size_t pos = block.find(marker, searchPos);
            
            // Ensure valid location directive (not part of another word)
            while (pos != std::string::npos) {
                size_t afterPos = pos + marker.length();
                if (afterPos < block.length()) {
                    char nextChar = block[afterPos];
                    if (nextChar == ' ' || nextChar == '\t' || nextChar == '\r' || 
                        nextChar == '\n' || nextChar == '{')
                        break;
                } else {
                    break;
                }
                pos = block.find(marker, pos + 1);
            }
            
            // Extract location block content
            if (pos != std::string::npos) {
                size_t bracePos = block.find('{', pos);
                if (bracePos != std::string::npos) {
                    size_t innerStart = bracePos + 1;
                    size_t innerEnd = innerStart;
                    int braceCount = 1;

                    // Find matching closing brace
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
                        iss.clear();
                        iss.seekg(static_cast<std::streampos>(innerEnd));
                        continue;
                    }
                }
            }
        }
    }
    return true;
}// Search and extract server blocks from configuration text
bool ConfigParser::searchServers(const std::string& text,
                                  std::vector<ServerConfigStruct>& outServers) {
    size_t pos = 0;
    
    while ((pos = text.find("server", pos)) != std::string::npos) {
        // Check if 'server' is standalone keyword
        if (pos > 0 && text[pos - 1] != '\n') {
            pos += 6;
            continue;
        }

        // Find opening brace
        size_t bracePos = pos + 6;
        while (bracePos < text.length() && std::isspace(text[bracePos]))
            bracePos++;

        if (bracePos < text.length() && text[bracePos] == '{') {
            size_t blockStart = bracePos + 1;
            size_t blockEnd = blockStart;
            int braceCount = 1;
            
            // Find matching closing brace
            while (blockEnd < text.length() && braceCount > 0) {
                if (text[blockEnd] == '{') braceCount++;
                else if (text[blockEnd] == '}') braceCount--;
                blockEnd++;
            }
            
            if (braceCount == 0) {
                std::string serverBlock = text.substr(blockStart, blockEnd - blockStart - 1);
                ServerConfigStruct srv;
                if (fillServer(serverBlock, srv))
                    outServers.push_back(srv);
            } else {
                s_lastError = "Mismatched braces in server block.";
                return false;
            }
        }
        pos += 6;
    }
    return true;
}

// Main parser: read config file and return ServerConfig objects
std::vector<ServerConfig> ConfigParser::RunParser(const char * config_file) {
    try {
        std::vector<ServerConfigStruct> serversParsing;
        std::vector<ServerConfig> servers;
        std::string text;

        // Read configuration file
        if (!readFile(config_file, text)) {
            std::cerr << "Error: " << LastError() << std::endl;
            return std::vector<ServerConfig>();
        }
        
        // Search for server blocks
        if (!searchServers(text, serversParsing)) {
            std::cerr << "Error: " << LastError() << std::endl;
            return std::vector<ServerConfig>();
        }
        
        // Convert to ServerConfig objects
        for (size_t i = 0; i < serversParsing.size(); i++)
            servers.push_back(ServerConfig(serversParsing[i], i));
        
        return servers;
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return std::vector<ServerConfig>();
    }
}
