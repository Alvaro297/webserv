#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Response.hpp"

#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>

class CGIHandler {
public:
    CGIHandler(const std::map<std::string, std::string>& cgiExts, const std::string& root,
               const std::string& serverName = "localhost", const std::string& serverPort = "80");

    // Ejecuta el script CGI referido por la request. Devuelve un Response generado a partir
    // de la salida del CGI o de un error si algo falla.
    Response handle(const Request& req);

private:
    std::map<std::string, std::string> _cgiExts; // mapping .php -> /usr/bin/php-cgi, etc.
    std::string _root;
    std::string _serverName;
    std::string _serverPort;

    Response buildErrorResponse(int code, const std::string& message) const;
    // pathInfo: portion of the URL after the script filename (e.g. for /a/script.py/foo -> /foo)
    std::vector<std::string> buildEnv(const Request& req, const std::string& scriptPath, const std::string& pathInfo = std::string()) const;
    int executeCGIProcess(const std::string& scriptPath, const std::string& interpreter,
                          const std::vector<std::string>& env, const std::string& body,
                          std::string& out) const;
};

#endif
