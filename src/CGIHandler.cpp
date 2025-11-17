#include "../inc/CGIHandler.hpp"

// Constructor: Initialize CGI handler with configuration
CGIHandler::CGIHandler(const std::map<std::string, std::string>& cgiExts, const std::string& root,
                       const std::string& serverName, const std::string& serverPort)
    : _cgiExts(cgiExts), _root(root), _serverName(serverName), _serverPort(serverPort) {}

// Build HTTP error response with given status code and message
Response CGIHandler::buildErrorResponse(int code, const std::string& message) const {
    Response r;
    r.setStatus(code, message);
    r.setError(code);
    r.setHeader("Content-Type", "text/html");
    std::ostringstream ss;
    ss << "<h2>" << code << " " << message << "</h2>";
    r.setBody(ss.str());
    return r;
}

// Convert header name to CGI environment variable name format (uppercase, - to _)
static std::string to_env_name(const std::string& s) {
    std::string result = s;
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = (result[i] == '-') ? '_' : std::toupper(static_cast<unsigned char>(result[i]));
    return result;
}

// Build CGI environment variables from request data
std::vector<std::string> CGIHandler::buildEnv(const Request& req, const std::string& scriptPath, 
                                               const std::string& pathInfo) const {
    std::vector<std::string> env;
    const std::map<std::string, std::string>& headers = req.getHeaders();
    env.reserve(12 + headers.size());
    
    // Standard CGI variables
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("SCRIPT_NAME=" + scriptPath);
    env.push_back("SCRIPT_FILENAME=" + scriptPath);
    env.push_back("SERVER_NAME=" + _serverName);
    env.push_back("SERVER_PORT=" + _serverPort);
    env.push_back("SERVER_PROTOCOL=" + req.getVersion());
    env.push_back("QUERY_STRING=");
    env.push_back("DOCUMENT_ROOT=" + _root);

    // Path info
    if (!pathInfo.empty()) {
        env.push_back("PATH_INFO=" + pathInfo);
        env.push_back("PATH_TRANSLATED=" + _root + pathInfo);
    }

    // Content headers (special case)
    if (headers.count("Content-Length")) 
        env.push_back("CONTENT_LENGTH=" + headers.at("Content-Length"));
    if (headers.count("Content-Type")) 
        env.push_back("CONTENT_TYPE=" + headers.at("Content-Type"));

    // Convert remaining HTTP headers to HTTP_* environment variables
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
         it != headers.end(); ++it) {
        if (it->first != "Content-Length" && it->first != "Content-Type")
            env.push_back("HTTP_" + to_env_name(it->first) + "=" + it->second);
    }
    return env;
}

// Execute CGI script in a child process with timeout handling
int CGIHandler::executeCGIProcess(const std::string& scriptPath, const std::string& interpreter,
                                   const std::vector<std::string>& env, const std::string& body,
                                   std::string& out) const {
    int inpipe[2], outpipe[2];
    
    // Create pipes for stdin and stdout
    if (pipe(inpipe) == -1 || pipe(outpipe) == -1) 
        return -1;

    pid_t pid = fork();
    if (pid < 0) { 
        close(inpipe[0]); 
        close(inpipe[1]); 
        close(outpipe[0]); 
        close(outpipe[1]); 
        return -1; 
    }

    // Child process: execute CGI script
    if (pid == 0) {
        // Redirect stdin/stdout to pipes
        dup2(inpipe[0], STDIN_FILENO);
        dup2(outpipe[1], STDOUT_FILENO);
        close(inpipe[0]); close(inpipe[1]); close(outpipe[0]); close(outpipe[1]);

        // Change to script directory for relative path resolution
        std::string scriptFileName = scriptPath;
        size_t lastSlash = scriptPath.rfind('/');
        if (lastSlash != std::string::npos) {
            std::string dir = scriptPath.substr(0, lastSlash);
            scriptFileName = scriptPath.substr(lastSlash + 1);
            chdir(dir.c_str());
        }

        // Build argv array
        std::vector<char*> argv;
        if (!interpreter.empty()) { 
            argv.push_back(const_cast<char*>(interpreter.c_str())); 
            argv.push_back(const_cast<char*>(scriptFileName.c_str())); 
        } else {
            argv.push_back(const_cast<char*>(scriptFileName.c_str()));
        }
        argv.push_back(NULL);

        // Build environment array
        std::vector<char*> envp;
        envp.reserve(env.size() + 1);
        for (size_t i = 0; i < env.size(); ++i) 
            envp.push_back(const_cast<char*>(env[i].c_str()));
        envp.push_back(NULL);

        // Execute script
        execve(interpreter.empty() ? scriptFileName.c_str() : interpreter.c_str(), 
               &argv[0], &envp[0]);
        _exit(127);
    }

    // Parent process: send input and read output
    close(inpipe[0]); close(outpipe[1]);
    
    // Write request body to script's stdin
    ssize_t to_write = static_cast<ssize_t>(body.size());
    const char* buf = body.c_str();
    while (to_write > 0) {
        ssize_t w = write(inpipe[1], buf, to_write);
        if (w <= 0) break;
        buf += w; to_write -= w;
    }
    close(inpipe[1]);

    // Set non-blocking mode for timeout implementation
    int flags = fcntl(outpipe[0], F_GETFL, 0);
    if (flags != -1) 
        fcntl(outpipe[0], F_SETFL, flags | O_NONBLOCK);

    // Read output with timeout
    char tmp[4096];
    std::ostringstream oss;
    int status = 0;
    const int CGI_TIMEOUT_SEC = 5;
    time_t start = time(NULL);
    
    while (true) {
        ssize_t r = read(outpipe[0], tmp, sizeof(tmp));
        if (r > 0) {
            oss.write(tmp, r);
            continue;
        }
        
        // Check if child process exited
        pid_t w = waitpid(pid, &status, WNOHANG);
        if (w == pid) {
            // Read remaining data
            while ((r = read(outpipe[0], tmp, sizeof(tmp))) > 0) 
                oss.write(tmp, r);
            break;
        }
        
        // Handle timeout
        if (time(NULL) - start > CGI_TIMEOUT_SEC) {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            close(outpipe[0]);
            out = oss.str();
            return -1;
        }
        
        // Prevent busy waiting
        usleep(10000);
    }
    
    close(outpipe[0]);
    out = oss.str();
    return status;
}

// Main CGI request handler
Response CGIHandler::handle(const Request& req) {
    const std::string path = req.getPath();
    
    // Validate request path
    if (path.empty() || path[0] != '/')
        return buildErrorResponse(400, "Bad Request");

    // Find the script file by traversing path segments
    struct stat st;
    std::string matchedScriptFs;
    size_t searchPos = path.size();

	// Traverse backwards to find the longest matching script file
    while (searchPos > 0) {
        std::string candidateFs = _root + path.substr(0, searchPos);
        
        // Check if candidate is a regular file
        if (stat(candidateFs.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
            matchedScriptFs = candidateFs;
            break;
        }
        
        if (searchPos == 1) break;
        size_t prev = path.rfind('/', searchPos - 1);
        if (prev == std::string::npos) break;
        searchPos = prev;
    }

    if (matchedScriptFs.empty())
        return buildErrorResponse(404, "Not Found");

    // Determine interpreter from file extension
    size_t pos = matchedScriptFs.rfind('.');
    std::string ext = (pos == std::string::npos) ? "" : matchedScriptFs.substr(pos);
    std::string interpreter;
    std::map<std::string, std::string>::const_iterator it = _cgiExts.find(ext);
    if (it != _cgiExts.end()) 
        interpreter = it->second;

    // Extract PATH_INFO (extra path after script name)
    std::string pathInfo;
    if (searchPos < path.size()) {
        pathInfo = path.substr(searchPos);
        if (!pathInfo.empty() && pathInfo[0] != '/') 
            pathInfo = "/" + pathInfo;
    }

    // Execute CGI script
    std::string output;
    int status = executeCGIProcess(matchedScriptFs, interpreter, 
                                   buildEnv(req, matchedScriptFs, pathInfo), 
                                   req.getBody(), output);
    if (status == -1)
        return buildErrorResponse(504, "Gateway Time");

    // Parse CGI output (headers and body)
    size_t split = output.find("\r\n\r\n");
    if (split == std::string::npos)
        split = output.find("\n\n");

    Response res;
    
    // No headers found, treat all as body
    if (split == std::string::npos) {
        res.setStatus(200, "OK");
        res.setHeader("Content-Type", "text/html");
        res.setBody(output);
        return res;
    }

    // Split headers and body
    std::string rawHeaders = output.substr(0, split);
    std::string body = output.substr(split + (output[split] == '\r' ? 4 : 2));

    // Parse CGI headers
    std::istringstream hs(rawHeaders);
    std::string line;
    int statusCode = 200;
    std::string statusMsg = "OK";
    
    while (std::getline(hs, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.resize(line.size() - 1);
        if (line.empty()) continue;
            
        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
            
        std::string key = line.substr(0, colon);
        std::string value = (colon + 1 < line.size()) ? line.substr(colon + 1) : "";
        if (!value.empty() && value[0] == ' ') value.erase(0, 1);
            
        if (key == "Status") { 
            std::istringstream ss(value);
            ss >> statusCode;
            std::getline(ss, statusMsg);
            if (!statusMsg.empty() && statusMsg[0] == ' ') statusMsg.erase(0, 1);
        } else {
            res.setHeader(key, value);
        }
    }

    // Build final response
    res.setStatus(statusCode, statusMsg);
    if (res.genResponseString().empty())
        res.setHeader("Content-Type", "text/html");
    res.setBody(body);
    return res;
}
