#include "../inc/CGIHandler.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
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

std::vector<std::string> CGIHandler::buildEnv(const Request& req, const std::string& scriptPath) const {
    std::vector<std::string> env;
    env.reserve(8 + req.getHeaders().size());
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("SCRIPT_NAME=" + scriptPath);
    env.push_back("SERVER_NAME=" + _serverName);
    env.push_back("SERVER_PORT=" + _serverPort);
    env.push_back("SERVER_PROTOCOL=" + req.getVersion());
    env.push_back("QUERY_STRING=");

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

        std::vector<char*> argv;
        if (!interpreter.empty()) { argv.push_back(const_cast<char*>(interpreter.c_str())); argv.push_back(const_cast<char*>(scriptPath.c_str())); }
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

    // read output
    char tmp[4096];
    std::ostringstream oss;
    ssize_t r;
    while ((r = read(outpipe[0], tmp, sizeof(tmp))) > 0) oss.write(tmp, r);
    close(outpipe[0]);

    int status = 0; waitpid(pid, &status, 0);
    out = oss.str();
    return status;
}

Response CGIHandler::handle(const Request& req) {
    const std::string path = req.getPath();
    if (path.empty() || path[0] != '/') return buildErrorResponse(400, "Bad Request");

    const std::string scriptFsPath = _root + path;
    struct stat st; if (stat(scriptFsPath.c_str(), &st) != 0) return buildErrorResponse(404, "Not Found");

    const size_t pos = scriptFsPath.rfind('.');
    const std::string ext = (pos==std::string::npos) ? std::string() : scriptFsPath.substr(pos);
    std::string interpreter;
    std::map<std::string, std::string>::const_iterator it = _cgiExts.find(ext);
    if (it != _cgiExts.end()) interpreter = it->second;

    std::string output; int status = executeCGIProcess(scriptFsPath, interpreter, buildEnv(req, scriptFsPath), req.getBody(), output);
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
<<<<<<< HEAD
        if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1, 1);
=======
        if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
>>>>>>> master
        if (line.empty()) continue;
        size_t c = line.find(':');
        if (c == std::string::npos) continue;
        std::string hk = line.substr(0, c);
        std::string hv = (c+1 < line.size()) ? line.substr(c+1) : std::string();
        if (!hv.empty() && hv[0] == ' ') hv.erase(0,1);
        if (hk == "Status") { std::istringstream ss(hv); ss >> statusCode; std::getline(ss, statusMsg);
        if (!statusMsg.empty() && statusMsg[0]==' ') statusMsg.erase(0,1); }
        else res.setHeader(hk, hv);
    }

    res.setStatus(statusCode, statusMsg);
    if (res.genResponseString().empty()) res.setHeader("Content-Type", "text/html");
    res.setBody(body);
    return res;
}

bool CGIHandler::supportsExtension(const std::string& ext) const { return _cgiExts.count(ext) > 0; }
