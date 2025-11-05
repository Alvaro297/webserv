#include "../inc/autoindex.hpp"

#include <sys/types.h>
#include <dirent.h>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

static bool isDotOrDotDot(const std::string& name) {
    return name == "." || name == "..";
}

std::string generateAutoindexHtml(const std::string& dirPath, const std::string& requestPath) {
    std::ostringstream oss;
    oss << "<html><head><meta charset=\"utf-8\"><title>Index of " << requestPath << "</title>";
    oss << "<style>body{font-family:Arial,Helvetica,sans-serif}pre{font-family:monospace}</style>";
    oss << "</head><body><h1>Index of " << requestPath << "</h1><hr><pre>";

    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        oss << "Could not open directory.";
        oss << "</pre><hr></body></html>";
        return oss.str();
    }

    std::vector<std::string> entries;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        std::string name = ent->d_name;
        if (isDotOrDotDot(name)) continue;
        entries.push_back(name);
    }
    closedir(dir);

    std::sort(entries.begin(), entries.end());

    for (size_t i = 0; i < entries.size(); ++i) {
        std::string name = entries[i];
        std::string href = requestPath;
        if (href.empty() || href[0] != '/') href = "/" + href;
        if (href[href.size()-1] != '/') href.push_back('/');
        href += name;

        // Try to detect if it's a directory to append '/'
        struct stat st;
        std::string full = dirPath;
        if (full.empty() || full[full.size()-1] != '/') full.push_back('/');
        full += name;
        if (stat(full.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            href.push_back('/');
            name.push_back('/');
        }

        oss << "<a href=\"" << href << "\">" << name << "</a>\n";
    }

    oss << "</pre><hr></body></html>";
    return oss.str();
}
