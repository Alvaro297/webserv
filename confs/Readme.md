# Configuración `.conf` — Referencia Rápida

## Estructura básica

server {
    host 0.0.0.0;
    port 8080;
    server_name localhost;
    root www/;
    index index.html;
    client_max_body_size 5M;
    
    error_page 404 www/errors/404.html;
    error_page 500 www/errors/500.html;
    
    location / {
        root www/;
        index index.html;
        methods GET HEAD;
        autoindex on;
    }
    
    location /cgi {
        root www/cgi/;
        methods GET POST HEAD;
        cgi_enable on;
        cgi_extension .py /usr/bin/python3;
    }
    
    location /upload {
        root www/upload/;
        methods GET POST HEAD;
        upload_enable on;
        upload_store www/uploads/;
    }
    
    location /delete {
        root www/delete/;
        methods GET DELETE HEAD;
    }
    
    location /redirect {
        return 302 https://example.com;
    }
}
```

## Directivas principales

### Bloque `server`
- `host` - IP de escucha (0.0.0.0 = todas)
- `port` - Puerto del servidor
- `server_name` - Nombre del host
- `root` - Directorio raíz
- `index` - Archivo por defecto
- `client_max_body_size` - Tamaño máximo (ej: 5M, 10M)
- `error_page <código> <ruta>` - Páginas de error personalizadas

### Bloque `location`
- `root` - Directorio para esta ruta
- `index` - Archivo por defecto
- `methods` - Métodos permitidos (GET, POST, DELETE, HEAD)
- `autoindex on/off` - Listado de directorios
- `upload_enable on/off` - Permitir subida de archivos
- `upload_store` - Carpeta destino para uploads
- `cgi_enable on/off` - Ejecutar scripts CGI
- `cgi_extension <ext> <intérprete>` - Extensión y ejecutable CGI
- `return <código> <url>` - Redirección (301/302)

## Notas
- Las rutas pueden ser relativas al ejecutable
- `methods` controla qué verbos HTTP acepta cada location
- Las redirecciones 301 son permanentes (se cachean), 302 son temporales
- CGI requiere la ruta completa al intérprete (ej: `/usr/bin/python3`)

