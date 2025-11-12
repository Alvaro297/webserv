#include "../inc/CGIHandler.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <sstream>

CGIHandler::CGIHandler(const std::map<std::string, std::string>& cgiExts, const std::string& root,
                       const std::string& serverName, const std::string& serverPort)
    : _cgiExts(cgiExts), _root(root), _serverName(serverName), _serverPort(serverPort) {}

Response CGIHandler::buildErrorResponse(int code, const std::string& message) const {
    Response r;
    r.setStatus(code, message);
    r.setHeader("Content-Type", "text/html");
    std::ostringstream ss;
    ss << "<h2>" << code << " " << message << "</h2>";
    r.setBody(ss.str());
    return r;
}

static char _env_map_char(char c) {
    if (c == '-') return '_';
    return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

static std::string to_env_name(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), _env_map_char);
    return s;
}

std::vector<std::string> CGIHandler::buildEnv(const Request& req, const std::string& scriptPath, const std::string& pathInfo) const {
    std::vector<std::string> env;
    env.reserve(12 + req.getHeaders().size());
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    // SCRIPT_NAME: URL path to the script (we provide the scriptPath as given)
    env.push_back("SCRIPT_NAME=" + scriptPath);
    // SCRIPT_FILENAME: full path on filesystem
    env.push_back(std::string("SCRIPT_FILENAME=") + scriptPath);
    env.push_back("SERVER_NAME=" + _serverName);
    env.push_back("SERVER_PORT=" + _serverPort);
    env.push_back("SERVER_PROTOCOL=" + req.getVersion());

    // QUERY_STRING: Request may not expose it; default to empty
    std::string qstr;
    // If Request exposes getQueryParam only, leave empty; caller can extend Request to provide full query string.
    env.push_back(std::string("QUERY_STRING=") + qstr);

    // PATH_INFO and PATH_TRANSLATED
    if (!pathInfo.empty()) {
        env.push_back(std::string("PATH_INFO=") + pathInfo);
        env.push_back(std::string("PATH_TRANSLATED=") + _root + pathInfo);
    }

    // DOCUMENT_ROOT
    env.push_back(std::string("DOCUMENT_ROOT=") + _root);

    const std::map<std::string, std::string>& headers = req.getHeaders();
    if (headers.count("Content-Length")) env.push_back("CONTENT_LENGTH=" + headers.at("Content-Length"));
    if (headers.count("Content-Type")) env.push_back("CONTENT_TYPE=" + headers.at("Content-Type"));

    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        if (it->first == "Content-Length" || it->first == "Content-Type") continue;
        env.push_back(std::string("HTTP_") + to_env_name(it->first) + "=" + it->second);
    }
    return env;
}

int CGIHandler::executeCGIProcess(const std::string& scriptPath, const std::string& interpreter,
                                  const std::vector<std::string>& env, const std::string& body,
                                  std::string& out) const {
    int inpipe[2], outpipe[2];
    if (pipe(inpipe) == -1 || pipe(outpipe) == -1) return -1;

    pid_t pid = fork();
    if (pid < 0) { close(inpipe[0]); close(inpipe[1]); close(outpipe[0]); close(outpipe[1]); return -1; }

    if (pid == 0) {
        dup2(inpipe[0], STDIN_FILENO);
        dup2(outpipe[1], STDOUT_FILENO);
        close(inpipe[0]); close(inpipe[1]); close(outpipe[0]); close(outpipe[1]);

        // Change working directory to the script directory so relative paths inside script work
        size_t lastSlash = scriptPath.rfind('/');
        if (lastSlash != std::string::npos) {
            std::string dir = scriptPath.substr(0, lastSlash);
            chdir(dir.c_str());
        }

        std::vector<char*> argv;
        if (!interpreter.empty()) { 
            argv.push_back(const_cast<char*>(interpreter.c_str())); 
            argv.push_back(const_cast<char*>(scriptPath.c_str())); 
        }
        else argv.push_back(const_cast<char*>(scriptPath.c_str()));
        argv.push_back(NULL);

        std::vector<char*> envp;
        envp.reserve(env.size() + 1);
        for (size_t i = 0; i < env.size(); ++i) envp.push_back(const_cast<char*>(env[i].c_str()));
        envp.push_back(NULL);

        if (!interpreter.empty()) execve(interpreter.c_str(), &argv[0], &envp[0]);
        else execve(scriptPath.c_str(), &argv[0], &envp[0]);
        _exit(127);
    }

    // parent
    close(inpipe[0]); close(outpipe[1]);
    // write body
    ssize_t to_write = static_cast<ssize_t>(body.size());
    const char* buf = body.c_str();
    while (to_write > 0) {
        ssize_t w = write(inpipe[1], buf, to_write);
        if (w <= 0) break;
        buf += w; to_write -= w;
    }
    close(inpipe[1]);

    // Make stdout pipe non-blocking so we can implement a timeout while waiting for child
    int flags = fcntl(outpipe[0], F_GETFL, 0);
    if (flags != -1) fcntl(outpipe[0], F_SETFL, flags | O_NONBLOCK);

    // read output with timeout while waiting for child
    char tmp[4096];
    std::ostringstream oss;
    int status = 0;
    const int CGI_TIMEOUT_SEC = 5; // timeout for CGI scripts
    time_t start = time(NULL);
    while (true) {
        ssize_t r = read(outpipe[0], tmp, sizeof(tmp));
        if (r > 0) {
            oss.write(tmp, r);
            continue;
        }
        // check if child exited
        pid_t w = waitpid(pid, &status, WNOHANG);
        if (w == pid) {
            // read any remaining data
            while ((r = read(outpipe[0], tmp, sizeof(tmp))) > 0) oss.write(tmp, r);
            break;
        }
        // timeout
        if (time(NULL) - start > CGI_TIMEOUT_SEC) {
            // kill child and return an error
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            close(outpipe[0]);
            out = oss.str();
            return -1;
        }
        // avoid busy loop
        usleep(10000);
    }
    close(outpipe[0]);
    out = oss.str();
    return status;
}

Response CGIHandler::handle(const Request& req) {
    const std::string path = req.getPath();
    if (path.empty() || path[0] != '/') return buildErrorResponse(400, "Bad Request");
    // Try to find the longest existing file prefix in the requested path so we can extract PATH_INFO.
    // e.g. request /cgi-bin/script.py/foo -> prefer /cgi-bin/script.py as script and /foo as PATH_INFO
    struct stat st;
    std::string matchedScriptFs;
    std::string matchedScriptUrl;
    bool found = false;
    // iterate over possible split points from end towards the start
    size_t searchPos = path.size();
    while (true) {
        // candidate url is path[0:searchPos)
        std::string candidateUrl = path.substr(0, searchPos);
        std::string candidateFs = _root + candidateUrl;
        if (stat(candidateFs.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
            matchedScriptFs = candidateFs;
            matchedScriptUrl = candidateUrl;
            found = true;
            break;
        }
        if (searchPos == 1) break; // reached the leading '/'
        size_t prev = path.rfind('/', searchPos - 1);
        if (prev == std::string::npos) break;
        searchPos = prev;
    }

    if (!found) return buildErrorResponse(404, "Not Found");

    const std::string scriptFsPath = matchedScriptFs;
    const size_t pos = scriptFsPath.rfind('.');
    const std::string ext = (pos==std::string::npos) ? std::string() : scriptFsPath.substr(pos);
    std::string interpreter;
    std::map<std::string, std::string>::const_iterator it = _cgiExts.find(ext);
    if (it != _cgiExts.end()) interpreter = it->second;

    // compute PATH_INFO: remainder of URL after matchedScriptUrl
    std::string pathInfo;
    if (matchedScriptUrl.size() < path.size()) {
        pathInfo = path.substr(matchedScriptUrl.size());
        // ensure it starts with '/'
        if (pathInfo.empty() || pathInfo[0] != '/') pathInfo = std::string("/") + pathInfo;
    }

    std::string output; int status = executeCGIProcess(scriptFsPath, interpreter, buildEnv(req, scriptFsPath, pathInfo), req.getBody(), output);
    if (status == -1) return buildErrorResponse(500, "Internal Server Error");

    size_t split = output.find("\r\n\r\n");
    if (split == std::string::npos) split = output.find("\n\n");

    Response res;
    if (split == std::string::npos) {
        res.setStatus(200, "OK");
        res.setHeader("Content-Type", "text/html");
        res.setBody(output);
        return res;
    }

    std::string rawHeaders = output.substr(0, split);
    std::string body = output.substr(split + (output[split]=='\r' ? 4 : 2));

    std::istringstream hs(rawHeaders);
    std::string line; int statusCode = 200; std::string statusMsg = "OK";
    while (std::getline(hs, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1, 1);
        if (line.empty()) continue;
            size_t c = line.find(':');
            if (c == std::string::npos) continue;
            std::string hk = line.substr(0, c);
            std::string hv = (c+1 < line.size()) ? line.substr(c+1) : std::string();
            if (!hv.empty() && hv[0] == ' ') hv.erase(0,1);
            if (hk == "Status") { 
                std::istringstream ss(hv); ss >> statusCode; std::getline(ss, statusMsg);
                if (!statusMsg.empty() && statusMsg[0]==' ') 
                    statusMsg.erase(0,1);
            }
            else res.setHeader(hk, hv);
    }

    res.setStatus(statusCode, statusMsg);
    if (res.genResponseString().empty()) res.setHeader("Content-Type", "text/html");
    res.setBody(body);
    return res;
}