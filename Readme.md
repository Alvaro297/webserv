**Persona A Alvaro**

**Persona B Dani**

**Persona C Mario**

Si terminais algo marcarlo ya sea en negrita con un listo o tachandolo (No lo quiteis)

👥 Reparto del equipo
Persona	Rol	Módulos principales
🧠 A (Core Networking)	Infraestructura del servidor y gestión de sockets/eventos	Server, Client, PollManager
⚙️ B (HTTP Engine)	Parsing de peticiones, respuestas y status codes	Request, Response, HttpHandler
🗂️ C (Config + CGI)	Configuración .conf, filesystem, CGI y autoindex	ConfigParser, CGIHandler, FileManager
📆 Semana 1 — Fundamentos y base del servidor
Persona	Tareas
A	- Implementar clase Server que abra sockets y escuche.
**- Aceptar conexiones (bloqueante al principio).**
**- Montar estructura del bucle principal (poll() o kqueue()).**
- Estructurar proyecto: carpetas, Makefile, namespaces.
B	- Diseñar clases Request y Response (headers, body, status code).
- Entender y resumir RFC 2616 (HTTP/1.1).
- Implementar parsing básico de una request simple (GET).
C	- Crear parser básico de configuración (server { ... }).
- Leer archivo .conf y almacenar hosts, puertos, root, etc.
- Documentar formato de config para el equipo.

🎯 Objetivo semana 1:
Servidor levanta, acepta conexiones y responde “Hello World” en localhost:8080.
Estructura de código lista.

📆 Semana 2 — HTTP funcional
Persona	Tareas
A	- Convertir el servidor a no bloqueante.
- Implementar poll() o kqueue() para manejar varios clientes.
- Crear clase Client con buffer de lectura/escritura.
- Enviar datos a B (interfaz de lectura de request).
B	- Completar parsing HTTP: método, headers, body, versión.
- Implementar GET correctamente con rutas.
- Crear respuestas dinámicas con headers y códigos (200, 404, etc.).
C	- Completar ConfigParser (múltiples servidores, locations).
- Asociar rutas con configuración (root, index, error_page).
- Servir archivos estáticos (index.html, imágenes, etc.).

🎯 Objetivo semana 2:
Servidor puede servir páginas estáticas a múltiples clientes concurrentes.
Configuración personalizada funciona.

📆 Semana 3 — Métodos y CGI
Persona	Tareas
A	- Pulir gestión de sockets (timeouts, cierres limpios).
- Añadir control de eventos de lectura/escritura.
- Estabilizar el bucle principal.
B	- Implementar POST y DELETE.
- Gestionar uploads simples y borrado de archivos.
- Implementar gestión de errores HTTP (400, 403, 405, 500...).
C	- Implementar CGIHandler:
→ Ejecutar scripts (PHP, Python).
→ Leer stdout del proceso CGI.
→ Integrar con Response de B.
- Implementar autoindex y redirecciones (return 301 /path).

🎯 Objetivo semana 3:
Servidor sirve archivos, procesa POST y DELETE, y ejecuta CGI funcional.
Pasa tests básicos del subject.

📆 Semana 4 — Refinamiento, testing y limpieza
Persona	Tareas
A	- Revisar fugas de memoria y cierres de sockets.
- Añadir logs de conexiones (debug).
- Testear con ab o wrk.
B	- Revisar todas las respuestas HTTP y headers.
- Comprobar conformidad con RFC y casos edge (long headers, empty body).
- Documentar cómo añadir nuevos métodos si hiciera falta.
C	- Probar configuración con varios vhosts.
- Revisar CGI, autoindex y error pages.
- Documentar docker-compose o scripts de test si hacéis entorno reproducible.

🎯 Objetivo semana 4:
Servidor estable, sin leaks, pasando todos los tests del subject.
Preparado para presentación (norma + funcionamiento).

⚙️ Extras recomendados

Reunión corta diaria (15 min) → estado, bloqueos, decisiones.

Git branches claras:

network/, http/, config/

Merge solo tras revisión cruzada.

Tester propio: script bash con curl -v para 20–30 casos típicos.

