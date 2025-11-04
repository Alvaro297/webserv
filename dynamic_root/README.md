Dynamic root demo para el proyecto 42 webserv

Contenido:
- index.html: página principal con enlaces y botones de prueba.
- upload.html: formulario multipart para subir archivos al endpoint /upload.
- cgi-bin/hello.py: script CGI de ejemplo (requiere configurar .py -> /usr/bin/python3 y CGI enabled).
- test.js: utilidades para crear/borrar archivos con fetch (POST/DELETE) y llamar al CGI.
- uploads/: carpeta vacía (local) y también existe `uploads/` en la raíz del proyecto donde Handler guarda archivos multipart.

Cómo probar (resumen):
1) Asegúrate de que tu configuración del server habilita CGI para la extensión .py y apunta a python3 (/usr/bin/python3).
2) Ejecuta el servidor con la configuración que apunte el `root` a `default_root/` o al `dynamic_root` donde corresponda.
3) Visita /dynamic_root/ y prueba las acciones: subir archivos, ejecutar CGI, crear/borrar archivos vía AJAX.

Notas técnicas:
- El formulario de upload envía al endpoint /upload; el Handler del proyecto guarda archivos en la carpeta `uploads/` relativa al proceso, por eso se crea `uploads/` en la raíz del proyecto.
- El CGI requiere que el servidor invoque el intérprete (p. ej. /usr/bin/python3) o esté bien configurado en la sección cgi_extensions del config.
