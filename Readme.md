**Persona A Alvaro** - Networking (COMPLETADO ✅)

**Persona B Dani** - HTTP Engine (COMPLETADO ✅) 

**Persona C Mario** - Config + Files + CGI (COMPLETADO ✅)

## 🎯 **PROGRESO ACTUALIZADO: ~90%** 

**ESTADO ACTUAL:** El webserv está funcionalmente **COMPLETO**. Todas las funcionalidades mandatorias están implementadas:
- ✅ Servidor HTTP/1.1 no bloqueante con poll()
- ✅ Múltiples conexiones simultáneas
- ✅ GET/POST/DELETE implementados
- ✅ Configuración completa (.conf parsing)
- ✅ Servir archivos estáticos + MIME types
- ✅ CGI completo (múltiples lenguajes)
- ✅ Upload de archivos (multipart)
- ✅ Autoindex
- ✅ Error handling robusto

**PENDIENTE:** Solo testing exhaustivo y posibles bugs menores.

---

---

## 👥 Reparto del equipo

| Persona | Rol | Módulos principales |
|---------|-----|---------------------|
| 🧠 **A** (Core Networking) | Infraestructura del servidor y gestión de sockets/eventos | Server, Client, PollManager |
| ⚙️ **B** (HTTP Engine) | Parsing de peticiones, respuestas y status codes | Request, Response, HttpHandler |
| 🗂️ **C** (Config + CGI) | Configuración .conf, filesystem, CGI y autoindex | ConfigParser, CGIHandler, FileManager |

---

## 📆 Semana 1 — Fundamentos y base del servidor

### Persona A (Networking) - Alvaro
- **✅ Implementar clase Server que abra sockets y escuche**
- **✅ Aceptar conexiones (bloqueante al principio)**
- **✅ Montar estructura del bucle principal (poll() o kqueue())**
- **✅ Estructurar proyecto: carpetas, Makefile, namespaces**
- **✅ Crear clase Client con fd y buffers básicos**
- **✅ Sockets non-blocking con fcntl(O_NONBLOCK)**
- **✅ SO_REUSEADDR para reutilizar puertos**

### Persona B (HTTP Engine) - Dani
- **✅ Diseñar clases Request y Response (headers, body, status code)**
- **✅ Entender y resumir RFC 2616 (HTTP/1.1)**
- **✅ Implementar parsing completo HTTP (GET/POST/DELETE)**
- **✅ Parsing de headers, body, query strings**
- **✅ Validación de Content-Length**
- **✅ MIME types detection**
- **✅ Interfaz Request/Response definida**

### Persona C (Config + Files) - Mario
- **✅ Crear parser básico de configuración (server { ... })**
- **✅ Leer archivo .conf y almacenar hosts, puertos, root, etc.**
- **✅ Documentar formato de config para el equipo**
- **✅ Clase ServerConfig con getters/setters**
- **✅ Parser completo (locations, error_pages, client_max_body_size)**
- **✅ Handler básico para servir archivos**
- **✅ CGIHandler implementado (fork/execve/pipes)**
- **✅ Autoindex implementado**

### 🎯 Objetivo semana 1:
**✅ Servidor levanta, acepta conexiones y detecta datos**  
**✅ Estructura de código lista**  
**✅ Responde "Hello World" básico** (hardcoded, esperando parser HTTP de Persona B)

---

## 📆 Semana 2 — HTTP funcional y multiplexación

### Persona A (Networking) - Alvaro
- **✅ Convertir el servidor a no bloqueante**
- **✅ Implementar poll() dinámico para listeners + clientes**
- **✅ Crear clase Client con readBuffer/writeBuffer**
- **✅ Detectar request HTTP completa (\r\n\r\n) con Content-Length**
- **✅ isListener() para distinguir listeners de clientes**
- **✅ Integración completa con Handler (processRequest → Handler → Response)**
- **✅ Implementar writeClient() con send() para enviar respuestas**
- **✅ Limpiar readBuffer después de procesar**
- **✅ Manejar errores EAGAIN/EWOULDBLOCK correctamente**
- **✅ Timeouts con lastActivity tracking**
- **✅ POLLOUT para escritura cuando hay datos pendientes**

### Persona B (HTTP Engine) - Dani
- **✅ Completar parsing HTTP: método, URI, versión, headers, body**
- **✅ Implementar GET/POST/DELETE correctamente**
- **✅ Crear respuestas dinámicas con headers y códigos (200, 404, etc.)**
- **✅ Validar requests malformadas (400 Bad Request)**
- **✅ Manejar Content-Length para POST**
- **✅ Query string parsing (?param=value)**
- **✅ Multipart form data parsing completo**
- **✅ File type detection automático (MIME types)**

### Persona C (Config + Files) - Mario
- **✅ Completar ConfigParser (múltiples servidores, locations)**
- **✅ Asociar rutas con configuración (root, index, error_page)**
- **✅ Servir archivos estáticos (Handler::handleGET implementado)**
- **✅ Implementar búsqueda de archivos index**
- **✅ MIME types básicos (getMimeTypeFromPath completo)**
- **✅ Manejar permisos de lectura (403 Forbidden)**
- **✅ Upload de archivos (multipart handling)**
- **✅ CGI completo (variables entorno, pipes, timeout)**
- **✅ Autoindex HTML generation**

### 🎯 Objetivo semana 2:
**✅ Servidor puede servir páginas estáticas a múltiples clientes concurrentes**
**✅ Configuración personalizada funciona**
**✅ GET funcional con archivos del disco**
**✅ POST y DELETE implementados**
**✅ Multipart upload funciona**

---

## 📆 Semana 3 — Métodos HTTP completos y CGI

### Persona A (Networking) - Alvaro
- **✅ Implementar timeouts de conexión (detectar clientes inactivos)**
- **✅ Manejar POLLOUT para escritura cuando buffer está lleno**
- **✅ Gestión limpia de cierres (close() y erase() de _client)**
- **✅ Manejar SIGPIPE (ignorar, no crashear)**
- **✅ poll() con timeout para chequeos regulares**
- **✅ lastActivity tracking en Client class**
- **✅ Integración completa Server ↔ Handler ↔ Request/Response**

### Persona B (HTTP Engine) - Dani  
- **✅ Implementar POST con body completo**
- **✅ Implementar DELETE (borrado de archivos)**
- **✅ Validar client_max_body_size**
- **✅ Implementar códigos de error: 400, 403, 404, 405, 413, 500, 501, 505**
- **✅ Parsing de Query String (?param=value)**
- **✅ Gestionar uploads de archivos (multipart/form-data completo)**
- **✅ File saving con saveMultipartFile()**

### Persona C (Config + CGI) - Mario
- **✅ Implementar CGIHandler completo:**
  - **✅ Ejecutar scripts (múltiples lenguajes configurables)**
  - **✅ fork() + execve() + pipes**
  - **✅ Leer stdout del proceso CGI**
  - **✅ Parsear headers CGI (Status:, Content-Type:)**
  - **✅ Timeout para scripts que se cuelgan**
- **✅ Variables de entorno CGI (REQUEST_METHOD, QUERY_STRING, etc.)**
- **✅ Implementar autoindex (listado de directorios HTML)**
- **✅ Handler integrado (GET/POST/DELETE routing)**
- ⏳ Redirecciones (return 301/302)
- ⏳ Directiva limit_except (métodos permitidos por location)

### 🎯 Objetivo semana 3:
**✅ Servidor sirve archivos, procesa POST y DELETE**
**✅ CGI funcional con múltiples lenguajes**
**✅ Upload y download de archivos funciona**
**✅ Autoindex implementado**
⏳ Pasa todos los tests del subject (testing pendiente)

---

## 📆 Semana 4 — Refinamiento, testing y subject completo

### Persona A (Networking)
- ⏳ Revisar fugas de memoria (valgrind --leak-check=full)
- ⏳ Verificar cierres de sockets (lsof, netstat)
- ⏳ Testear con siege/ab/wrk (stress test 100+ clientes)
- **✅ Manejar SIGPIPE (ignorar, no crashear)** - ya implementado en Semana 3
- ⏳ Logs de debug (conexiones, errores, timeouts)
- **✅ Verificar que poll() se llama solo una vez por iteración** - verificado
- ⏳ Garantizar que el servidor NUNCA cuelga indefinidamente

### Persona B (HTTP Engine)
- ⏳ Revisar conformidad RFC 2616 (o HTTP/1.0 como referencia)
- ⏳ Casos edge: headers largos, body vacío, método desconocido
- ⏳ Validar con navegadores reales (Chrome, Firefox)
- ⏳ Probar con Postman/Insomnia (POST, DELETE)
- ⏳ Documentar API interna (cómo añadir métodos nuevos)

### Persona C (Config + CGI + Files)
- ⏳ Verificar error_page personalizadas
- ⏳ Revisar CGI con extensión elegida (.php o .py)
- ⏳ Testear autoindex y permisos
- ⏳ Documentar formato .conf completo con ejemplos
- ⏳ Proporcionar archivos de configuración y archivos de prueba

### Lo que falta Persona B
- **✅ buildFilePath() hardcoded
- ⏳ handleGET() NO maneja directorios
- **✅ Upload storage hardcoded
- ⏳ Location-based method validation
- **✅ Handler NO recibe ServerConfig

### Testing conjunto
- ⏳ Script de tests automatizado (bash + curl)
- ⏳ 20-30 casos típicos del subject
- ⏳ Tests de evaluadores anteriores
- ⏳ Preparar defensa: explicar arquitectura, decisiones, manejo de errores

### 🎯 Objetivo semana 4:
⏳ Servidor estable, sin leaks, sin crashes  
⏳ Pasa todos los tests del subject  
⏳ Preparado para evaluación

---

## ✅ Checklist completa del Subject (MANDATORY ONLY)

> **IMPORTANTE:** Esta sección contiene SOLO los requisitos OBLIGATORIOS del subject. Los bonus están en sección separada más abajo.

### HTTP/1.1 Core
- [ ] HTTP/1.1 compliant (RFC 2616 como referencia, HTTP/1.0 sugerido pero no obligatorio)
- [ ] Métodos: GET, POST, DELETE
- [ ] Response con status code correcto (200, 404, 500, etc.)
- [ ] Headers de request/response correctos
- [ ] Manejo de body en POST

### Configuración
- [x] Archivo .conf estilo nginx (Persona C)
- [x] Múltiples server blocks (Persona C)
- [x] listen (puerto) (Persona C)
- [x] root (directorio base) (Persona C)
- [x] index (archivos por defecto) (Persona C)
- [x] error_page (páginas de error personalizadas) (Persona C)
- [x] client_max_body_size (límite de tamaño - implementado en ServerConfig)
- [x] location blocks con: (Persona C)
  - [x] root o alias
  - [ ] limit_except (métodos permitidos)
  - [ ] return (redirecciones)
  - [ ] autoindex on/off
- [ ] **Ejecutable debe tomar [archivo de configuración] como argumento**
- [ ] **Proporcionar archivos de configuración y archivos de prueba**

> **Nota del subject sobre Virtual Hosting:** "We deliberately chose to offer only a subset of the HTTP RFC. In this context, the virtual host feature is considered out of scope. But you are allowed to implement it if you want." → **NO ES OBLIGATORIO**

### Archivos estáticos
- [x] Servir archivos HTML, CSS, JS, imágenes (Handler::handleGET)
- [x] MIME types correctos (getMimeTypeFromPath)
- [x] Búsqueda de index files (buildFilePath)
- [x] Autoindex (listado de directorio - generateAutoindexHtml)
- [x] Manejo de permisos (403 si no se puede leer)
- [x] **Servir una página web completamente estática**

### CGI (OBLIGATORIO - al menos 1 tipo)
- [x] Ejecutar CGI con múltiples extensiones configurables (CGIHandler)
- [x] Variables de entorno CGI correctas (buildEnv implementado):
  - REQUEST_METHOD
  - QUERY_STRING
  - CONTENT_LENGTH
  - CONTENT_TYPE
  - PATH_INFO
  - SCRIPT_FILENAME
- [x] Pipes stdin/stdout con proceso CGI
- [x] Parsear headers que devuelve el CGI (Status:, Content-Type:, etc.)
- [x] Timeout para CGI que se cuelga
- [x] **Manejo de respuestas CGI sin Content-Length** (EOF marca el final)
- [x] **Ejecutar CGI en el directorio correcto** (para rutas relativas)
- [x] **fork() SOLO para CGI** (no para otras cosas)

### Upload de archivos
- [x] POST con multipart/form-data (parseMultipartBody implementado)
- [x] Clientes pueden subir archivos (saveMultipartFile)
- [x] Guardar archivos en directorio configurado (upload_store)
- [x] Respetar client_max_body_size (validation implementada)

### Non-blocking I/O (CRÍTICO - SUSPENDES SI NO CUMPLES)
- [x] poll() o select() o kqueue() o epoll() (Persona A)
- [x] **Solo UNA llamada a poll/select/etc. por iteración** (Persona A - verificado)
- [x] Non-blocking sockets (fcntl O_NONBLOCK) (Persona A)
- [x] Manejo correcto de EAGAIN/EWOULDBLOCK (Persona A)
- [x] **Nunca leer/escribir en sockets sin pasar por poll()** (Persona A)
- [ ] **Archivos de disco regulares (open/read/write) NO requieren poll()** (permitido bloqueante)
- [x] **Monitoring simultáneo de lectura Y escritura** con poll() (Persona A)
- [x] **El servidor debe permanecer non-blocking en todo momento** (Persona A)

> **CRÍTICO del subject:** "I/O that can wait for data (sockets, pipes/FIFOs, etc.) must be non-blocking and driven by a single poll() (or equivalent). Calling read/recv or write/send on these descriptors without prior readiness will result in a grade of 0. Regular disk files are exempt."

### Robustez (CRÍTICO)
- [ ] **No crashear NUNCA** (stress test con siege/ab)
- [ ] **Sin leaks de memoria** (valgrind)
- [x] **Manejo de señales** (SIGPIPE ignorado) (Persona A)
- [x] **Timeouts de conexión** (requests no pueden colgar indefinidamente) (Persona A)
- [ ] **Servidor disponible en todo momento** (stress test)
- [ ] **Compatibilidad con navegadores estándar** (Chrome, Firefox, Safari)
- [ ] **No usar execve() para otro servidor web**
- [ ] **Manejar correctamente desconexiones de clientes**
- [ ] **Comprobar valor de errno PROHIBIDO después de read/write**

> **Importante:** "A request to your server should never hang indefinitely."

> **Importante:** "Checking the value of errno to adjust the server behaviour is strictly forbidden after performing a read or write operation."

### Códigos de error HTTP
- [ ] 200 OK
- [ ] 201 Created (POST exitoso)
- [ ] 204 No Content (DELETE exitoso)
- [ ] 301 Moved Permanently (redirect)
- [ ] 302 Found (redirect temporal)
- [ ] 400 Bad Request (request malformada)
- [ ] 403 Forbidden (sin permisos)
- [ ] 404 Not Found
- [ ] 405 Method Not Allowed (limit_except)
- [ ] 413 Payload Too Large (client_max_body_size)
- [ ] 500 Internal Server Error
- [ ] 501 Not Implemented (método no soportado)
- [ ] 505 HTTP Version Not Supported
- [ ] **Páginas de error por defecto si no se proporcionan**

### Puertos múltiples
- [x] **Servidor debe poder escuchar en múltiples puertos** (Persona A + C)
- [x] **Servir contenido diferente según el puerto** (Persona C - configuración)

### Compilación y Makefile
- [x] Flags: -Wall -Wextra -Werror
- [x] Reglas: all, clean, fclean, re
- [x] Compila sin warnings
- [x] C++98 compliant
- [x] NAME del ejecutable: webserv
- [x] **Uso:** `./webserv [configuration file]` (implementado en main.cpp)

---

## 🌟 BONUS PART (Sección oficial del Subject)

> **⚠️ IMPORTANTE:** Los bonus SOLO se evalúan si la parte MANDATORY está PERFECTA. No comiences bonus hasta que TODO lo de arriba esté 100% funcional y testeado sin errores.

> **Nota del subject:** "Here are some additional features you can implement:"

### 1. **Cookies y gestión de sesiones** 🍪
- [ ] Implementar `Set-Cookie:` en respuestas HTTP
- [ ] Leer header `Cookie:` en requests
- [ ] Gestión básica de sesiones (session ID)
- [ ] **Proporcionar ejemplos simples de uso** (requisito explícito del subject)

**Responsable sugerido:** Persona B (HTTP Engine)

**Dificultad:** Media (3-5 horas)

**Ejemplo práctico requerido:**
```http
Response del servidor:
HTTP/1.1 200 OK
Set-Cookie: sessionid=abc123; Path=/; HttpOnly

Request siguiente del cliente:
GET /profile HTTP/1.1
Cookie: sessionid=abc123
```

---

### 2. **Múltiples tipos de CGI** 🔧
- [ ] Soportar **más de un tipo** de CGI (PHP, Python, Perl)
- [ ] Configuración por extensión (.php, .py, .pl)
- [ ] Variables de entorno correctas para cada tipo
- [ ] Tests con scripts reales de cada lenguaje

> **Nota:** El mandatory solo requiere 1 tipo de CGI. Este bonus requiere **múltiples tipos** (al menos 2 o más).

**Responsable sugerido:** Persona C (Config + CGI)

**Dificultad:** Fácil-Media (2-4 horas si ya tienes 1 CGI funcionando)

**Ejemplo de configuración:**
```nginx
location ~ \.php$ {
    cgi_pass /usr/bin/php-cgi;
}
location ~ \.py$ {
    cgi_pass /usr/bin/python3;
}
location ~ \.pl$ {
    cgi_pass /usr/bin/perl;
}
```

---

## 📋 Mejoras opcionales (NO son bonus oficiales)

Estas características NO están en el subject como bonus, pero mejoran el servidor. **No las hagas hasta tener mandatory + bonus oficiales perfectos.**

- [ ] Chunked Transfer Encoding (request y response)
- [ ] Keep-Alive / Persistent connections (Connection: keep-alive)
- [ ] Content-Range (partial downloads / resume)
- [ ] If-Modified-Since (caching básico HTTP)
- [ ] Logs detallados con timestamps estilo nginx
- [ ] Compression gzip (Accept-Encoding / Content-Encoding)
- [ ] Autoindex mejorado (HTML bonito con iconos)
- [ ] Custom error pages mejoradas (variables dinámicas)
- [ ] Virtual Hosting completo (server_name + Host header)
- [ ] Límite de clientes simultáneos configurable

---

## ⚙️ Prácticas recomendadas

### Comunicación
- **Reunión corta diaria (15 min)** → estado, bloqueos, decisiones
- **Documentar decisiones importantes** en carpeta `/docs`
- **Interfaz clara entre A-B-C** (archivo INTERFACE.md)

### Git workflow
- **Branches claras:** `feature/network`, `feature/http`, `feature/config`
- **Merge solo tras code review** entre personas
- **Commits descriptivos:** "feat: add chunked encoding" no "fix stuff"

### Testing continuo
- **Tester propio:** script bash con curl -v para 20–30 casos
- **Valgrind diario:** `valgrind --leak-check=full ./webserv`
- **Stress test semanal:** `siege -c100 -r10 http://localhost:8080`

### Debugging
- **Logs con niveles:** DEBUG, INFO, ERROR
- **Compilar con -g** para gdb/lldb
- **Verificar fds abiertos:** `lsof -p $(pidof webserv)`

---

## 📚 Recursos útiles

- **RFC 2616 (HTTP/1.1):** https://www.rfc-editor.org/rfc/rfc2616
- **RFC 1945 (HTTP/1.0):** https://www.rfc-editor.org/rfc/rfc1945 (sugerido como referencia)
- **Nginx config docs:** https://nginx.org/en/docs/
- **CGI 1.1 spec:** https://www.rfc-editor.org/rfc/rfc3875
- **Beej's Guide to Network Programming:** https://beej.us/guide/bgnet/
- **HTTP status codes:** https://httpstatuses.com/

> **Consejo del subject:** "If you have a question about a specific behaviour, you can compare your program's behaviour with NGINX's."

---

## 🐛 Problemas comunes y soluciones

### "Address already in use"
```bash
sudo pkill -9 webserv
# O añadir SO_REUSEADDR al socket (ya implementado ✅)
```

### "poll() debe llamarse solo una vez por iteración"
```cpp
// ✅ Correcto:
while (true) {
    // preparar fds
    int ready = poll(&fds[0], fds.size(), timeout);  // UNA sola llamada
    // procesar eventos
}

// ❌ Incorrecto (nota 0):
while (true) {
    poll(...);  // Primera llamada
    // ...
    poll(...);  // Segunda llamada - PROHIBIDO
}
```

### "read/write sin poll() previo"
```cpp
// ❌ Incorrecto (nota 0):
recv(fd, buffer, size, 0);  // Sin verificar POLLIN antes

// ✅ Correcto:
if (fds[i].revents & POLLIN) {
    recv(fds[i].fd, buffer, size, 0);
}
```

### "Archivos de disco vs sockets"
```cpp
// ✅ Archivos regulares NO necesitan poll():
int fd = open("file.html", O_RDONLY);  // Bloqueante OK
read(fd, buffer, size);                 // Sin poll() OK
close(fd);

// ❌ Sockets/pipes SÍ necesitan poll():
int socket_fd = accept(...);
// NO HACER: read(socket_fd, ...) directamente
// CORRECTO: esperar POLLIN con poll()

```


