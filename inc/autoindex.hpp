#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include <string>

// Genera un HTML con el listado de archivos dentro de dirPath.
// requestPath es la parte de la URL solicitada (por ejemplo "/images/") usada para construir los enlaces.
std::string generateAutoindexHtml(const std::string& dirPath, const std::string& requestPath);

#endif
