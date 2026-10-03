# webserv - 42 School Project

HTTP/1.1 web server in C++98 from scratch.

## Overview

Web server that handles multiple concurrent connections using non-blocking I/O with `poll()`. Supports GET, POST, DELETE methods, CGI execution, file uploads, and virtual hosts.

## Features

- **HTTP Methods**: GET, POST, DELETE, HEAD
- **Non-blocking I/O**: Uses `poll()` for concurrent connections
- **Virtual hosts**: Multiple servers with custom configuration
- **CGI execution**: Python, Shell scripts
- **File uploads**: Multipart/form-data support
- **Autoindex**: Directory listings
- **Custom error pages**: 404, 500, 504
- **HTTP redirects**: 302, 303

## Project Structure

```
webserv/
├── src/
│   ├── main.cpp              # Entry point
│   ├── configParser.cpp      # Configuration parser
│   ├── Handler.cpp           # Request routing
│   ├── Request.cpp           # HTTP request parsing
│   ├── Response.cpp          # HTTP response building
│   ├── CGIHandler.cpp        # CGI execution
│   ├── autoindex.cpp         # Directory listings
│   └── server/
│       ├── Server.cpp        # Main server with poll()
│       ├── ServerConfig.cpp  # Configuration structures
│       └── Client.cpp        # Client connection handling
├── inc/                      # Header files
├── confs/
│   └── default.conf          # Example configuration
├── www/                      # Document root
│   ├── index.html
│   ├── cgi/                  # CGI scripts
│   ├── upload/               # Upload form
│   └── errors/               # Custom error pages
└── Makefile
```

## Compilation

```bash
make        # Compile
make re     # Recompile
make clean  # Remove objects
make fclean # Remove binary
```

## Usage

```bash
./webserv confs/default.conf
```

Server available at `http://localhost:8080`

### Configuration Example

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

### Testing

```bash
# GET request
curl http://localhost:8080/

# File upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload/

# Delete file
curl -X DELETE http://localhost:8080/uploads/test.txt

# CGI execution
curl http://localhost:8080/cgi/test.py
```

## Requirements

- GCC or Clang (C++98 compatible)
- Make
- Unix-like environment (Linux, macOS, WSL)

## Author

- **Mario Pico** (@Davter17)

## License

Part of the 42 school curriculum.
