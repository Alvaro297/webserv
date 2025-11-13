#!/usr/bin/env python3
import sys
import os

# Leer datos POST
content_length = os.environ.get('CONTENT_LENGTH', '0')
post_data = ''

if content_length and content_length != '0':
    post_data = sys.stdin.read(int(content_length))

# Parsear datos
username = ''
password = ''

if post_data:
    pairs = post_data.split('&')
    for pair in pairs:
        if '=' in pair:
            key, value = pair.split('=', 1)
            value = value.replace('+', ' ')
            if key == 'username':
                username = value
            elif key == 'password':
                password = value

# Generar respuesta HTML
html = f"""<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Login Result - ZZZ</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background: #f5f5f5;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            margin: 0;
        }}
        .result-box {{
            background: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            text-align: center;
        }}
        h1 {{ color: #333; margin-bottom: 20px; }}
        .info {{ 
            background: #f5f5f5;
            padding: 20px;
            border-radius: 8px;
            margin: 20px 0;
            text-align: left;
        }}
        .info p {{ margin: 10px 0; color: #333; }}
        .label {{ font-weight: bold; color: #4a90e2; }}
        a {{
            display: inline-block;
            margin-top: 20px;
            padding: 10px 20px;
            background: #4a90e2;
            color: white;
            text-decoration: none;
            border-radius: 8px;
            transition: background 0.3s;
        }}
        a:hover {{ background: #357abd; }}
    </style>
</head>
<body>
    <div class="result-box">
        <h1>✅ Datos Recibidos</h1>
        <div class="info">
            <p><span class="label">Usuario:</span> {username}</p>
            <p><span class="label">Contraseña:</span> {password}</p>
        </div>
        <a href="/cgi/login.html">← Volver</a>
    </div>
</body>
</html>"""

print("Content-Type: text/html\n")
print(html)
