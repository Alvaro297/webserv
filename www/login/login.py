#!/usr/bin/env python3
import sys
import os

# Leer los datos POST del stdin
content_length = os.environ.get('CONTENT_LENGTH', '0')
post_data = ''

if content_length and content_length != '0':
    post_data = sys.stdin.read(int(content_length))

# Parsear los datos (formato: username=valor&password=valor)
username = ''
password = ''

if post_data:
    pairs = post_data.split('&')
    for pair in pairs:
        if '=' in pair:
            key, value = pair.split('=', 1)
            # URL decode básico (reemplazar + por espacio y %XX por caracteres)
            value = value.replace('+', ' ')
            if key == 'username':
                username = value
            elif key == 'password':
                password = value

# Generar la respuesta HTML
output = "Content-Type: text/html\n\n"
output += f"Usuario: {username}\nContraseña: {password}"

print(output, end='')
