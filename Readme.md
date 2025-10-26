**Persona A Alvaro**

**Persona B Dani**

**Persona C Mario**

Si terminais algo marcarlo ya sea en negrita con un listo o tachandolo (No lo quiteis)

---

## 👥 Reparto del equipo

| Persona | Rol | Módulos principales |
|---------|-----|---------------------|
| 🧠 **A** (Core Networking) | Infraestructura del servidor y gestión de sockets/eventos | Server, Client, PollManager |
| ⚙️ **B** (HTTP Engine) | Parsing de peticiones, respuestas y status codes | Request, Response, HttpHandler |
| 🗂️ **C** (Config + CGI) | Configuración .conf, filesystem, CGI y autoindex | ConfigParser, CGIHandler, FileManager |

---

## 📆 Semana 1 — Fundamentos y base del servidor

### Persona A (Networking)
- **✅ Implementar clase Server que abra sockets y escuche**
- **✅ Aceptar conexiones (bloqueante al principio)**
- **✅ Montar estructura del bucle principal (poll() o kqueue())**
- **✅ Estructurar proyecto: carpetas, Makefile, namespaces**
- **✅ Crear clase Client con fd y buffers básicos**
- **✅ Sockets non-blocking con fcntl(O_NONBLOCK)**
- **✅ SO_REUSEADDR para reutilizar puertos**

### Persona B (HTTP Engine)
- Diseñar clases Request y Response (headers, body, status code)
- Entender y resumir RFC 2616 (HTTP/1.1)
- Implementar parsing básico de una request simple (GET)
- Definir interfaz entre A y B (cómo recibir raw request)

### Persona C (Config + Files)
- **✅ Crear parser básico de configuración (server { ... })**
- **✅ Leer archivo .conf y almacenar hosts, puertos, root, etc.**
- **✅ Documentar formato de config para el equipo**
- **✅ Clase ServerConfig con getters/setters**

### 🎯 Objetivo semana 1:
**✅ Servidor levanta, acepta conexiones y detecta datos**  
**✅ Estructura de código lista**  
⏳ Responde "Hello World" básico

---

## 📆 Semana 2 — HTTP funcional y multiplexación

### Persona A (Networking)
- **✅ Convertir el servidor a no bloqueante**
- **✅ Implementar poll() dinámico para listeners + clientes**
- **✅ Crear clase Client con readBuffer/writeBuffer**
- **✅ Detectar request HTTP completa (\r\n\r\n)**
- **✅ isListener() para distinguir listeners de clientes**
- ⏳ Pasar request completa a Persona B (integración pendiente)
- ⏳ Implementar writeClient() con send() para enviar respuestas
- ⏳ Limpiar readBuffer después de procesar
- ⏳ Manejar errores EAGAIN/EWOULDBLOCK correctamente

### Persona B (HTTP Engine)
- ⏳ Completar parsing HTTP: método, URI, versión, headers, body
- ⏳ Implementar GET correctamente con rutas
- ⏳ Crear respuestas dinámicas con headers y códigos (200, 404, etc.)
- ⏳ Validar requests malformadas (400 Bad Request)
- ⏳ Manejar Content-Length para POST
- ⏳ Definir estructura Request con métodos públicos

### Persona C (Config + Files)
- **✅ Completar ConfigParser (múltiples servidores, locations)**
- **✅ Asociar rutas con configuración (root, index, error_page)**
- ⏳ Servir archivos estáticos (index.html, imágenes, etc.)
- ⏳ Implementar búsqueda de archivos index
- ⏳ MIME types básicos (html, css, js, jpg, png)
- ⏳ Manejar permisos de lectura (403 Forbidden)

### 🎯 Objetivo semana 2:
⏳ Servidor puede servir páginas estáticas a múltiples clientes concurrentes  
⏳ Configuración personalizada funciona  
⏳ GET funcional con archivos del disco

---

## 📆 Semana 3 — Métodos HTTP completos y CGI

### Persona A (Networking)
- **✅ Implementar timeouts de conexión (detectar clientes inactivos)**
- ⏳ Manejar POLLOUT para escritura cuando buffer está lleno
- ⏳ Gestión limpia de cierres (close() y erase() de _client)
- ⏳ Keep-Alive: mantener conexión abierta para múltiples requests
- ⏳ Detectar Connection: close vs keep-alive
- ⏳ Estabilizar bucle principal (manejo de errores, logs)

### Persona B (HTTP Engine)
- ⏳ Implementar POST con body completo
- ⏳ Implementar DELETE (borrado de archivos)
- ⏳ Chunked Transfer Encoding (lectura y escritura)
- ⏳ Gestionar uploads de archivos (multipart/form-data básico)
- ⏳ Validar client_max_body_size
- ⏳ Implementar códigos de error: 400, 403, 404, 405, 413, 500, 501, 505
- ⏳ Parsing de Query String (?param=value)

### Persona C (Config + CGI)
- ⏳ Implementar CGIHandler:
  - Ejecutar scripts (PHP, Python, Perl)
  - fork() + execve() + pipes
  - Leer stdout del proceso CGI
  - Parsear headers CGI (Status:, Content-Type:)
  - Timeout para scripts que se cuelgan
- ⏳ Variables de entorno CGI (REQUEST_METHOD, QUERY_STRING, etc.)
- ⏳ Implementar autoindex (listado de directorios)
- ⏳ Redirecciones (return 301/302)
- ⏳ Directiva limit_except (métodos permitidos por location)

### 🎯 Objetivo semana 3:
⏳ Servidor sirve archivos, procesa POST y DELETE  
⏳ CGI funcional con al menos un lenguaje  
⏳ Pasa tests básicos del subject

---

## 📆 Semana 4 — Refinamiento, testing y subject completo

### Persona A (Networking)
- ⏳ Revisar fugas de memoria (valgrind --leak-check=full)
- ⏳ Verificar cierres de sockets (lsof, netstat)
- ⏳ Testear con siege/ab/wrk (stress test 100+ clientes)
- ⏳ Manejar SIGPIPE (ignorar, no crashear)
- ⏳ Logs de debug (conexiones, errores, timeouts)
- ⏳ Verificar que poll() se llama solo una vez por iteración

### Persona B (HTTP Engine)
- ⏳ Revisar conformidad RFC 2616 completa
- ⏳ Casos edge: headers largos, body vacío, método desconocido
- ⏳ Validar con navegadores reales (Chrome, Firefox)
- ⏳ Probar con Postman/Insomnia (POST, DELETE)
- ⏳ Documentar API interna (cómo añadir métodos nuevos)

### Persona C (Config + CGI + Files)
- ⏳ Probar configuración con virtual hosting (múltiples server_name)
- ⏳ Verificar error_page personalizadas
- ⏳ Revisar CGI con diferentes extensiones (.php, .py, .pl)
- ⏳ Testear autoindex y permisos
- ⏳ Documentar formato .conf completo con ejemplos

### Testing conjunto
- ⏳ Script de tests automatizado (bash + curl)
- ⏳ 20-30 casos típicos del subject
- ⏳ Tests de evaluadores anteriores
- ⏳ Docker compose para entorno reproducible (opcional)
- ⏳ Preparar defensa: explicar arquitectura, decisiones, manejo de errores

### 🎯 Objetivo semana 4:
⏳ Servidor estable, sin leaks, sin crashes  
⏳ Pasa todos los tests del subject  
⏳ Norminette (si aplica en tu campus)  
⏳ Preparado para evaluación

---

## ✅ Checklist completa del Subject (Mandatory)

### HTTP/1.1 Core
- [ ] HTTP/1.1 compliant (RFC 2616)
- [ ] Métodos: GET, POST, DELETE
- [ ] Response con status code correcto (200, 404, 500, etc.)
- [ ] Headers de request/response correctos
- [ ] Manejo de body en POST

### Configuración
- [ ] Archivo .conf estilo nginx
- [ ] Múltiples server blocks
- [ ] listen (puerto)
- [ ] server_name (virtual hosting)
- [ ] root (directorio base)
- [ ] index (archivos por defecto)
- [ ] error_page (páginas de error personalizadas)
- [ ] client_max_body_size (límite de tamaño)
- [ ] location blocks con:
  - root o alias
  - limit_except (métodos permitidos)
  - return (redirecciones)
  - autoindex on/off

### Virtual Hosting
- [ ] Múltiples server_name en mismo puerto
- [ ] Elegir server correcto según Host header
- [ ] Default server si no hay match

### Archivos estáticos
- [ ] Servir archivos HTML, CSS, JS, imágenes
- [ ] MIME types correctos (Content-Type)
- [ ] Búsqueda de index files
- [ ] Autoindex (listado de directorio)
- [ ] Manejo de permisos (403 si no se puede leer)

### CGI
- [ ] Ejecutar CGI con al menos una extensión (.php, .py, .pl)
- [ ] Variables de entorno CGI correctas:
  - REQUEST_METHOD
  - QUERY_STRING
  - CONTENT_LENGTH
  - CONTENT_TYPE
  - PATH_INFO
  - SCRIPT_FILENAME
- [ ] Pipes stdin/stdout con proceso CGI
- [ ] Parsear headers que devuelve el CGI
- [ ] Timeout para CGI que se cuelga

### Upload de archivos
- [ ] POST con multipart/form-data
- [ ] Guardar archivos en directorio configurado
- [ ] Respetar client_max_body_size

### Non-blocking I/O
- [ ] poll() o select() o kqueue() o epoll()
- [ ] Solo UNA llamada a poll/select/etc. por iteración
- [ ] Non-blocking sockets (fcntl O_NONBLOCK)
- [ ] Manejo correcto de EAGAIN/EWOULDBLOCK

### Robustez
- [ ] No crashear nunca (stress test con siege/ab)
- [ ] Sin leaks de memoria (valgrind)
- [ ] Manejo de señales (SIGPIPE ignorado)
- [ ] Timeouts de conexión
- [ ] Límite de clientes simultáneos (recomendado)

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

### Compilación y Makefile
- [ ] Flags: -Wall -Wextra -Werror
- [ ] Reglas: all, clean, fclean, re
- [ ] Compila sin warnings
- [ ] C++98 compliant

---

## 📋 Extras recomendados (no obligatorios, mejoran el servidor)

- [ ] Chunked Transfer Encoding (request y response)
- [ ] Keep-Alive / Persistent connections
- [ ] Content-Range (partial downloads)
- [ ] If-Modified-Since (caching básico)
- [ ] Logs detallados con timestamps
- [ ] Múltiples lenguajes CGI (PHP + Python + Perl)

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
- **Nginx config docs:** https://nginx.org/en/docs/
- **CGI 1.1 spec:** https://www.rfc-editor.org/rfc/rfc3875
- **Beej's Guide to Network Programming:** https://beej.us/guide/bgnet/
- **HTTP status codes:** https://httpstatuses.com/

---

## 🐛 Problemas comunes y soluciones

### "Address already in use"
```bash
sudo pkill -9 webserv
# O añadir SO_REUSEADDR al socket (ya implementado)
```

