# Readme_Mario — Estado, cambios y siguientes pasos

LO QUE TIENES:

✅ ConfigParser 100% completo (servers, locations, todas las directivas parseadas y validadas)
✅ Clase `ServerConfig` con getters/setters y flags (index, autoindex, upload, cgi, error_pages)
✅ Estructura para CGI (mapeo extensión -> binario + flag `cgi_enable`)
✅ Location-based configuration parsing (locations parseadas en el parser)
✅ Directivas configurables: error_pages, autoindex, upload_enable/upload_store (parser valida y crea `upload_store`)

LO QUE FALTA - CRÍTICO (≈40%):

❌ CGI execution (0% implementado) — fork/execve/pipes aún no existe (MANDATORY)
❌ Autoindex (directory listing) — no hay `opendir/readdir` ni generación de listado (MANDATORY)
❌ File serving con MIME types completos — mapa MIME básico presente, falta centralizar/ampliar y consistencia
❌ Index file search — solo "/" → index.html hardcoded; no búsqueda general de `ServerConfig::index`
❌ Static file permissions handling — solo checks básicos (stat/ifstream); falta distinguir EACCES/ENOENT y mapear 403/404/500
❌ Upload file storage en directorios configurados — parser crea/valida `upload_store`, pero `Handler` todavía usa ruta hardcoded y no respeta `upload_store` ni sanitiza filenames
