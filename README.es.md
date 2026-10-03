# webserv - Proyecto de 42 School

Servidor web HTTP/1.1 en C++98 desde cero.

## Descripción General

Servidor web que maneja múltiples conexiones concurrentes usando I/O no bloqueante con `poll()`. Soporta métodos GET, POST, DELETE, ejecución CGI, subida de archivos y virtual hosts.

## Características

- **Métodos HTTP**: GET, POST, DELETE, HEAD
- **I/O No bloqueante**: Usa `poll()` para conexiones concurrentes
- **Virtual hosts**: Múltiples servidores con configuración personalizada
- **Ejecución CGI**: Scripts Python, Shell
- **Subida de archivos**: Soporte multipart/form-data
- **Autoindex**: Listados de directorios
- **Páginas de error personalizadas**: 404, 500, 504
- **Redirecciones HTTP**: 302, 303

## Estructura del Proyecto

```
webserv/
├── src/
│   ├── main.cpp              # Punto de entrada
│   ├── configParser.cpp      # Parser de configuración
│   ├── Handler.cpp           # Enrutamiento de peticiones
│   ├── Request.cpp           # Parsing de peticiones HTTP
│   ├── Response.cpp          # Construcción de respuestas HTTP
│   ├── CGIHandler.cpp        # Ejecución CGI
│   ├── autoindex.cpp         # Listados de directorios
│   └── server/
│       ├── Server.cpp        # Servidor principal con poll()
│       ├── ServerConfig.cpp  # Estructuras de configuración
│       └── Client.cpp        # Manejo de conexiones de clientes
├── inc/                      # Archivos de cabecera
├── confs/
│   └── default.conf          # Configuración de ejemplo
├── www/                      # Raíz del documento
│   ├── index.html
│   ├── cgi/                  # Scripts CGI
│   ├── upload/               # Formulario de subida
│   └── errors/               # Páginas de error personalizadas
└── Makefile
```

## Compilación

```bash
make        # Compilar
make re     # Recompilar
make clean  # Eliminar objetos
make fclean # Eliminar binario
```

## Uso

```bash
./webserv confs/default.conf
```

Servidor disponible en `http://localhost:8080`

### Ejemplo de Configuración

```nginx
server {
    host 127.0.0.1;
    port 8080;
    root www/;
    index index.html;
    client_max_body_size 5M;

    location / {
        methods GET;
        autoindex on;
    }

    location /cgi {
        cgi_enable on;
        cgi_extension .py /usr/bin/python3;
    }

    location /upload {
        upload_enable on;
        upload_store www/uploads/;
    }

    error_page 404 www/errors/404.html;
}
```

### Pruebas

```bash
# Petición GET
curl http://localhost:8080/

# Subida de archivo
curl -X POST -F "file=@test.txt" http://localhost:8080/upload/

# Eliminar archivo
curl -X DELETE http://localhost:8080/uploads/test.txt

# Ejecución CGI
curl http://localhost:8080/cgi/test.py
```

## Requisitos

- GCC o Clang (compatible C++98)
- Make
- Entorno tipo Unix (Linux, macOS, WSL)

## Autor

- **Mario Pico** (@Davter17)

## Licencia

Parte del plan de estudios de 42 school.
