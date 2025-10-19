# Guía básica de configuración — `webserv.conf`

Este documento resume las principales configuraciones del archivo `.conf` del servidor.

---

## Estructura general

* Los bloques se definen con llaves `{ ... }`.
* Los comentarios comienzan con `#`.
* Cada directiva termina con `;`.

### Bloques principales

* **server { ... }** → Define un sitio o virtual host.
* **location <ruta> { ... }** → Define reglas específicas para una parte de la URL.

---

## Directivas del bloque `server`

| Directiva                        | Descripción                                        | Ejemplo                                   |                 |
| -------------------------------- | -------------------------------------------------- | ----------------------------------------- | --------------- |
| `listen <ip>:<puerto>`           | Define en qué IP y puerto escucha el servidor.     | `listen 127.0.0.1:8080;`                  |                 |
| `server_name <nombre>`           | Nombre del sitio para el encabezado `Host`.        | `server_name ejemplo.local;`              |                 |
| `root <ruta>`                    | Carpeta base de los archivos del sitio.            | `root /var/www/ejemplo;`                  |                 |
| `error_page <código> <ruta>`     | Página personalizada para errores.                 | `error_page 404 /errors/404.html;`        |                 |
| `client_max_body_size <tamaño>`  | Tamaño máximo del cuerpo de una petición.          | `client_max_body_size 10m;`               |                 |
| `autoindex on                    | off`                                               | Permite listar archivos en un directorio. | `autoindex on;` |
| `index <archivo>`                | Archivo que se sirve por defecto en un directorio. | `index index.html;`                       |                 |
| `upload_store <ruta>`            | Carpeta donde se guardan archivos subidos.         | `upload_store /uploads;`                  |                 |
| `cgi_extension <ext> <programa>` | Define un intérprete para archivos CGI.            | `.php /usr/bin/php-cgi;`                  |                 |

---

## Directivas del bloque `location`

| Directiva               | Descripción                             | Ejemplo                                       |                     |
| ----------------------- | --------------------------------------- | --------------------------------------------- | ------------------- |
| `methods <...>`         | Lista de métodos HTTP permitidos.       | `methods GET POST;`                           |                     |
| `return <código> <url>` | Redirige o devuelve una respuesta fija. | `return 301 /nueva;`                          |                     |
| `root <ruta>`           | Carpeta base solo para esa ruta.        | `root /var/www/static;`                       |                     |
| `autoindex on           | off`                                    | Activa o desactiva el listado de directorios. | `autoindex off;`    |
| `index <archivo>`       | Archivo por defecto dentro de la ruta.  | `index index.html;`                           |                     |
| `upload_enable on       | off`                                    | Permite o bloquea subidas de archivos.        | `upload_enable on;` |
| `upload_store <ruta>`   | Define dónde guardar subidas.           | `upload_store /var/www/uploads;`              |                     |
| `cgi_enable on          | off`                                    | Activa o no la ejecución de CGI.              | `cgi_enable on;`    |

---

## Funcionamiento básico

1. El servidor elige el bloque `server` según el puerto e IP.
2. Dentro de él, busca la `location` que coincida con la URL.
3. Combina `root` + URL para encontrar el archivo.
4. Si el archivo es un CGI, lo ejecuta; si es estático, lo sirve.
5. Si es un directorio, intenta servir el `index` o genera listado si `autoindex on`.
6. Usa las páginas de error definidas con `error_page` en caso de fallo.

---

## Ejemplo corto

```conf
server {
    listen 127.0.0.1:8080;
    server_name ejemplo.local;
    root /var/www/ejemplo;
    index index.html;
    error_page 404 /errors/404.html;

    location /static {
        methods GET;
        autoindex on;
    }

    location /upload {
        methods POST;
        upload_enable on;
        upload_store /var/www/uploads;
    }
}
```
