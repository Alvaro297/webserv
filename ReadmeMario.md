Completadas (19)

- Parser básico de .conf (server { ... }) — implementado (src/configParser.cpp).
- Leer .conf y guardar hosts/puertos/root — implementado.
- Documentar formato de config (confs/Readme.md, .completeConf) — presente.
- ServerConfig con getters/setters — implementado (src/server/ServerConfig.cpp).
- ConfigParser completo (múltiples servidores, locations) — implementado.
- Asociar rutas con configuración (root, index, error_page) — implementado (Server usa ServerConfig).
- Servir archivos estáticos básicos (GET, index para /) — Handler::handleGET sirve ficheros.
- MIME types básicos — getMimeTypeFromPath en Handler.cpp.
- Manejo permisos lectura (403) — Handler devuelve 403 si no se puede abrir.
- CGI: fork + execve + pipes (ejecución CGI básica) — implementado (src/CGIHandler.cpp).
- Leer stdout de CGI — implementado.
- Parsear headers CGI (Status, Content-Type) — implementado.
- Timeout CGI — añadido (lectura no bloqueante y killer tras ~5s). // hecho en CGIHandler
- Manejo de respuestas CGI sin Content-Length (leer hasta EOF) — implementado.
- Ejecutar CGI en directorio del script (chdir) — añadido.
- fork() solo para CGI (no para otras cosas) — la ejecución de child está confinada a CGIHandler.
- Upload multipart básico (guardar archivos) — Handler::handleMULT + saveMultipartFile escriben en uploads/.
- Respetar client_max_body_size — Server::readClient y getMaxBodySizeLocation usan client_max_body_size.
- Proveer archivos de configuración / tests: hay .completeConf, creé test_cgi.conf, y scripts en tests para probar. // creado y marcado.

Nota: se añadió recientemente soporte para PATH_INFO en CGI y un módulo de autoindex (ver secciones abajo).

Parcial (0.7)
- Variables de entorno CGI completas — parcialmente:
	Ya se exportan: REQUEST_METHOD, SCRIPT_NAME, SCRIPT_FILENAME, SERVER_NAME, SERVER_PORT, SERVER_PROTOCOL, CONTENT_LENGTH, CONTENT_TYPE y HTTP_* para headers.
	Se implementó: PATH_INFO (CGIHandler detecta el script real en la URL y pasa el resto como PATH_INFO) y PATH_TRANSLATED y DOCUMENT_ROOT.
	Pendiente: QUERY_STRING no se envía como cadena completa porque `Request` no expone aún la query original; añadir `getQueryString()` en `Request` resolvería esto.

No realizadas / pendientes (6)
- Búsqueda/uso completo del vector index desde ServerConfig (buscar varios nombres de index) — Handler actualmente usa una lista hardcoded (index.html, index.htm); falta integrar la lista de `ServerConfig`/location.
- Un-chunking de requests Transfer-Encoding: chunked antes de pasar a CGI — no implementado.
- Integrar upload_store de ServerConfig (Handler escribe en uploads/ hardcoded) — pendiente.
- Respectar la directiva `autoindex` por location/server — el módulo de autoindex está implementado y `Handler` genera listados cuando no hay index, pero aún no consulta la configuración para decidir si generar o no el listado.
- return (redirecciones) en locations — configuración parseada pero falta aplicar en runtime → pendiente.
- limit_except (controlar métodos permitidos por location) — pendiente o parcial (config parsing tiene struct pero enforcement incomplete).