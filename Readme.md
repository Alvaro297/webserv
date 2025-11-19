# **WebServ**

Este proyecto, **WebServ**, representa una implementación completa de un servidor HTTP desde cero desarrollado en la escuela **42**. El objetivo es crear un servidor web completamente funcional que cumple con los estándares HTTP/1.1, capaz de manejar múltiples clientes concurrentes, procesar diferentes tipos de peticiones y ejecutar scripts CGI. Inspirado en servidores web como **nginx** y **Apache**, este proyecto demuestra un conocimiento profundo de protocolos de red, arquitectura de servidores y programación de sistemas.

---

## **✅ Habilidades y Funcionalidades Principales**

La finalización de WebServ demuestra la capacidad para implementar un sistema de servidor web de nivel producción, integrando las siguientes características clave:

* **Arquitectura Non-Blocking con Poll**: Implementación de un servidor **event-driven** utilizando `poll()` para manejar **300+ conexiones concurrentes** de manera eficiente, demostrando conocimientos avanzados en programación de sistemas y I/O multiplexing.
* **Protocolo HTTP/1.1 Completo**: Soporte completo para métodos **GET, POST, DELETE, HEAD** con manejo correcto de headers, status codes y respuestas HTTP estándar.
* **Sistema de Configuración Avanzado**: Parser de configuración personalizado que permite **virtual hosts**, configuración por ubicaciones, límites de tamaño y personalización de páginas de error, similar a la flexibilidad de nginx.
* **Ejecución de Scripts CGI**: Implementación completa de **Common Gateway Interface** con soporte para múltiples lenguajes (.py, .sh, .php), incluyendo manejo de variables de entorno y redirección de entrada/salida.
* **Manejo de Archivos y Uploads**: Sistema robusto para **subida de archivos multipart**, gestión de directorios con autoindex y eliminación segura de recursos.

---

## **💡 Características Avanzadas Implementadas**

Se han incorporado funcionalidades que elevan el proyecto a un nivel de calidad empresarial y demuestran una comprensión profunda del desarrollo web backend:

* **Performance Optimizada**: Servidor capaz de mantener **250 requests/segundo** sostenidos con manejo eficiente de archivos de hasta **5MB** y validación automática de límites configurables.
* **Seguridad y Validación**: Protección contra **directory traversal**, validación de métodos HTTP por ubicación, y prevención de corrupción de scripts CGI durante operaciones POST.
* **Sistema de Redirecciones**: Soporte completo para **redirecciones HTTP 302** configurables, permitiendo arquitecturas web complejas y gestión de URLs.
* **Autoindex Inteligente**: Generación automática de **listados de directorios HTML** con detección inteligente de archivos index y fallback configurable.

---

## **🛠️ Tecnologías y Habilidades Aplicadas**

* **Lenguaje de Programación**: Desarrollado íntegramente en **C++98**, demostrando dominio de programación orientada a objetos, gestión de memoria y uso de STL containers.
* **Programación de Red**: Implementación de **sockets TCP/IP**, manejo de conexiones persistentes, parsing de HTTP headers y gestión de timeouts.
* **Arquitectura de Software**: Diseño **modular y escalable** con separación clara de responsabilidades (Parser, Handler, Server, Response) siguiendo principios SOLID.
* **Testing y Quality Assurance**: Desarrollo de **4 suites de testing** comprehensivas (básico, extendido, stress, concurrencia extrema) que validan más de **100 escenarios** diferentes.

---

## **🏆 ¿Qué demuestra la finalización de este proyecto?**

Completar WebServ es una prueba tangible de las siguientes competencias técnicas de alto nivel:

* **Ingeniería de Software**: Capacidad para diseñar y implementar sistemas complejos con arquitectura escalable y mantenible, aplicando patrones de diseño y buenas prácticas de desarrollo.
* **Programación de Sistemas**: Dominio profundo de **system calls**, manejo de procesos, señales, file descriptors y programación concurrente en entornos Unix/Linux.
* **Conocimientos de Red y Protocolos**: Comprensión completa del **stack TCP/IP**, protocolo HTTP, y capacidad para implementar servidores de red de alto rendimiento.
* **Performance y Optimización**: Habilidad para escribir código eficiente capaz de manejar **carga de producción** con múltiples usuarios concurrentes y optimización de recursos del sistema.

---

## **📈 Métricas de Performance Validadas**

* **Conexiones Concurrentes**: 300+ usuarios simultáneos
* **Throughput**: 250 requests/segundo sostenidos
* **Uploads**: 30 archivos de 100KB en paralelo (1.13s)
* **CGI Performance**: 50 scripts concurrentes en 1.11s
* **Stress Testing**: 200 requests GET simultáneos en 0.79s

---

## **🚀 Cómo Ejecutarlo**

### **Requisitos del Sistema**
- Sistema **Linux** o **WSL** (Windows Subsystem for Linux)
- Compilador **g++** compatible con C++98
- Make instalado

### **Pasos de Instalación**
```bash
# 1. Clonar el repositorio
git clone https://github.com/Alvaro297/webserv.git
cd webserv

# 2. Compilar el proyecto
make

# 3. Configurar el servidor (opcional)
# Editar confs/zzz.conf para cambiar host/puerto si es necesario

# 4. Ejecutar el servidor
./webserv confs/zzz.conf
```

### **Acceso al Servidor**
Una vez iniciado, el servidor estará disponible en:
- **URL Principal**: `http://localhost:9000`
- **CGI Scripts**: `http://localhost:9000/cgi/`
- **Subida de Archivos**: `http://localhost:9000/upload/`
- **Listado de Uploads**: `http://localhost:9000/uploads/`

---

## **🌐 ¿Cómo Funciona Nuestro Servidor Web?**

### **🏠 Piensa en Nuestro Servidor Como un Restaurante Digital**

Imagina que nuestro servidor web es como un **restaurante muy eficiente** que puede atender a **cientos de clientes al mismo tiempo**. Aquí te explicamos cómo funciona de manera sencilla:

### **📋 1. El Maitre (Configuración)**
```
Cuando arrancas el servidor, primero lee un "manual de instrucciones" 
donde está escrito:
├─ "Atiende en la mesa 9000" (puerto)
├─ "Los archivos están en la cocina 'zzz/'" (carpeta de archivos)  
├─ "Máximo 5MB por plato" (tamaño de archivos)
└─ "En la zona /cgi hay cocineros especiales" (scripts dinámicos)
```

### **🚪 2. La Recepción (Conexiones de Clientes)**
```
Tu navegador (cliente) llega al restaurante:
🌐 Cliente: "¡Hola! Quiero ver el menú de localhost:9000"
🏠 Servidor: "¡Bienvenido! Te asigno una mesa y tomo tu pedido"
```

### **📝 3. Tomando el Pedido (Procesando Requests)**

Cuando pides algo en el navegador, el servidor entiende **4 tipos de pedidos**:

#### **🍽️ GET - "Quiero ver algo"**
```
👤 "Quiero ver la página principal"
🏠 → Busca el archivo index.html
🏠 → Te lo sirve en una bandeja bonita (respuesta HTTP)

👤 "Quiero ver qué hay en la carpeta /uploads"  
🏠 → Si está permitido, te muestra una lista como un menú
🏠 → Si no, te dice "Acceso denegado"
```

#### **📤 POST - "Quiero subir algo"**
```
👤 "Quiero subir una foto de mi gato"
🏠 → Recibe el archivo
🏠 → Lo guarda en la carpeta correcta (/uploads)
🏠 → Te confirma "¡Foto guardada correctamente!"
```

#### **🗑️ DELETE - "Quiero borrar algo"**
```
👤 "Quiero eliminar esa foto borrosa"
🏠 → Verifica que tienes permiso
🏠 → Elimina el archivo
🏠 → Te confirma "¡Archivo eliminado!"
```

### **🧙‍♂️ 4. Los Cocineros Mágicos (CGI Scripts)**

A veces pides algo que **no está precocinado**, sino que hay que **prepararlo al momento**:

```
👤 "Quiero rellenar un formulario de contacto"
🏠 → "Esto necesita un cocinero especial"
🏠 → Llama al cocinero Python 🐍
🐍 → Cocina tu pedido con ingredientes frescos
🐍 → Te devuelve una página personalizada con tu nombre
```

### **⚡ 5. ¿Por Qué Es Tan Rápido?**

**Nuestro restaurante es súper eficiente porque:**

- **🔄 Un Camarero Multitarea**: En lugar de tener un camarero por mesa (como otros servidores), tenemos **un súper camarero** que puede atender **300 mesas a la vez** sin cansarse.

- **📋 Sistema Organizado**: 
  ```
  Cliente A: "Quiero una pizza" → En proceso...
  Cliente B: "Quiero una ensalada" → Servido inmediatamente  
  Cliente C: "Quiero pasta" → En proceso...
  ↳ El camarero no espera que termine la pizza para atender otras mesas
  ```

- **🏃‍♂️ Sin Esperas Innecesarias**: Si algo tarda (como preparar un script), el servidor sigue atendiendo otras mesas mientras tanto.

### **🛡️ 6. Seguridad del Restaurante**

```
✅ Solo se sirven platos del menú (métodos HTTP permitidos)
✅ No puedes entrar a la cocina privada (protección de directorios)  
✅ Máximo 5MB por plato (evita que alguien pida algo gigante)
✅ Los cocineros no pueden estropear otros platos (CGI protegido)
```

### **🎯 Resumen Simple**

**Tu WebServ es como tener un restaurante digital que:**
- ✨ Atiende **cientos de clientes** simultáneamente
- 🚀 Sirve **páginas web, archivos y aplicaciones** dinámicas
- 📱 Funciona desde **cualquier navegador** o aplicación
- 🔒 Es **seguro y configurable** para diferentes necesidades
- ⚡ Es **súper rápido** porque está optimizado para eficiencia

**En términos prácticos:** Puedes usar este servidor para hospedar tu página web, subir archivos, ejecutar formularios dinámicos, o cualquier cosa que haría un servidor web profesional como Apache o nginx, ¡pero construido completamente desde cero por ti!