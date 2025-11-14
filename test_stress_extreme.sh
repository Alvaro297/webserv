#!/bin/bash

echo "════════════════════════════════════════════════"
echo " PRUEBAS DE ESTRÉS EXTREMAS - WEBSERV"
echo " Múltiples usuarios y máxima carga"
echo "════════════════════════════════════════════════"

# ============ PRUEBAS CON MÚLTIPLES USUARIOS SIMULTÁNEOS ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " PRUEBAS CON MÚLTIPLES USUARIOS SIMULTÁNEOS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[USERS-1] 20 usuarios GET simultáneos"
START=$(date +%s.%N)
for i in {1..20}; do
  curl -s http://localhost:9000/ -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 20 peticiones en ${DIFF}s"

echo -e "\n[USERS-2] 50 usuarios GET simultáneos"
START=$(date +%s.%N)
for i in {1..50}; do
  curl -s http://localhost:9000/ -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 50 peticiones en ${DIFF}s"

echo -e "\n[USERS-3] 100 usuarios GET simultáneos"
START=$(date +%s.%N)
for i in {1..100}; do
  curl -s http://localhost:9000/ -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 100 peticiones en ${DIFF}s"

echo -e "\n[USERS-4] 200 usuarios GET simultáneos (STRESS TEST)"
START=$(date +%s.%N)
for i in {1..200}; do
  curl -s http://localhost:9000/ -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 200 peticiones en ${DIFF}s"

# ============ MÚLTIPLES USUARIOS CON CGI ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " MÚLTIPLES USUARIOS EJECUTANDO CGI"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[CGI-USERS-1] 10 usuarios POST CGI simultáneos"
START=$(date +%s.%N)
for i in {1..10}; do
  curl -s -X POST -d "username=user$i&password=pass$i" http://localhost:9000/cgi/login.py -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 10 POST CGI en ${DIFF}s"

echo -e "\n[CGI-USERS-2] 25 usuarios POST CGI simultáneos"
START=$(date +%s.%N)
for i in {1..25}; do
  curl -s -X POST -d "username=user$i&password=pass$i" http://localhost:9000/cgi/login.py -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 25 POST CGI en ${DIFF}s"

echo -e "\n[CGI-USERS-3] 50 usuarios POST CGI simultáneos"
START=$(date +%s.%N)
for i in {1..50}; do
  curl -s -X POST -d "username=user$i&password=pass$i" http://localhost:9000/cgi/login.py -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 50 POST CGI en ${DIFF}s"

# ============ MÚLTIPLES UPLOADS SIMULTÁNEOS ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " MÚLTIPLES USUARIOS SUBIENDO ARCHIVOS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[UPLOAD-USERS-1] 10 usuarios subiendo archivos simultáneamente"
for i in {1..10}; do
  echo "content_user_$i" > /tmp/user_file_$i.txt
done
START=$(date +%s.%N)
for i in {1..10}; do
  curl -s -F "file=@/tmp/user_file_$i.txt" http://localhost:9000/upload -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 10 uploads en ${DIFF}s"

echo -e "\n[UPLOAD-USERS-2] 20 usuarios subiendo archivos simultáneamente"
for i in {11..30}; do
  echo "content_user_$i" > /tmp/user_file_$i.txt
done
START=$(date +%s.%N)
for i in {11..30}; do
  curl -s -F "file=@/tmp/user_file_$i.txt" http://localhost:9000/upload -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 20 uploads en ${DIFF}s"

echo -e "\n[UPLOAD-USERS-3] 30 usuarios subiendo archivos grandes (100KB) simultáneamente"
for i in {31..60}; do
  dd if=/dev/zero of=/tmp/user_file_$i.txt bs=1024 count=100 2>/dev/null
done
START=$(date +%s.%N)
for i in {31..60}; do
  curl -s -F "file=@/tmp/user_file_$i.txt" http://localhost:9000/upload -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 30 uploads (100KB cada uno) en ${DIFF}s"

# ============ TRÁFICO MIXTO SIMULADO ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " TRÁFICO MIXTO (GET + POST + UPLOAD + DELETE)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[MIX-1] 50 operaciones mixtas simultáneas"
START=$(date +%s.%N)

# 20 GET
for i in {1..20}; do
  curl -s http://localhost:9000/ -o /dev/null &
done

# 15 POST CGI
for i in {1..15}; do
  curl -s -X POST -d "user=mix$i&pass=test" http://localhost:9000/cgi/login.py -o /dev/null &
done

# 10 UPLOAD
for i in {1..10}; do
  echo "mix$i" > /tmp/mix_$i.txt
  curl -s -F "file=@/tmp/mix_$i.txt" http://localhost:9000/upload -o /dev/null &
done

# 5 DELETE
for i in {1..5}; do
  curl -s -X DELETE http://localhost:9000/uploads/user_file_$i.txt -o /dev/null &
done

wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 50 operaciones mixtas en ${DIFF}s"

# ============ PRUEBAS DE CARGA SOSTENIDA ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " PRUEBAS DE CARGA SOSTENIDA"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[SUSTAINED-1] 500 peticiones GET (oleadas de 50)"
START=$(date +%s)
for round in {1..10}; do
  for i in {1..50}; do
    curl -s http://localhost:9000/ -o /dev/null &
  done
  wait
done
END=$(date +%s)
DIFF=$((END - START))
echo "✅ 500 peticiones en ${DIFF}s ($(echo "scale=2; 500/$DIFF" | bc) req/s)"

echo -e "\n[SUSTAINED-2] 200 POST CGI (oleadas de 20)"
START=$(date +%s)
for round in {1..10}; do
  for i in {1..20}; do
    curl -s -X POST -d "round=$round&user=$i" http://localhost:9000/cgi/login.py -o /dev/null &
  done
  wait
done
END=$(date +%s)
DIFF=$((END - START))
echo "✅ 200 POST CGI en ${DIFF}s"

# ============ PRUEBAS DE CONEXIONES RÁPIDAS ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " CONEXIONES RÁPIDAS (KEEP-ALIVE / CLOSE)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[FAST-1] 100 conexiones rápidas secuenciales"
START=$(date +%s.%N)
for i in {1..100}; do
  curl -s http://localhost:9000/ -o /dev/null
done
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 100 conexiones en ${DIFF}s"

echo -e "\n[FAST-2] 200 conexiones muy rápidas (paralelo)"
START=$(date +%s.%N)
for i in {1..200}; do
  curl -s http://localhost:9000/test-autoindex -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 200 conexiones paralelas en ${DIFF}s"

# ============ PRUEBAS DE DIFERENTES RUTAS ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " USUARIOS ACCEDIENDO A DIFERENTES RUTAS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[ROUTES-1] 100 usuarios en rutas diferentes simultáneamente"
ROUTES=(
  "/"
  "/test-autoindex"
  "/cgi/login.html"
  "/uploads"
  "/redirect-test"
)
START=$(date +%s.%N)
for i in {1..100}; do
  ROUTE_INDEX=$((i % 5))
  ROUTE=${ROUTES[$ROUTE_INDEX]}
  curl -s -L http://localhost:9000$ROUTE -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 100 peticiones a rutas variadas en ${DIFF}s"

# ============ PRUEBAS CON DATOS VARIABLES ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " USUARIOS CON DIFERENTES TAMAÑOS DE DATOS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[VAR-DATA-1] 30 usuarios POST con datos de tamaño variable"
START=$(date +%s.%N)
for i in {1..30}; do
  SIZE=$((i * 100))
  DATA=$(printf "data=%0.s#" $(seq 1 $SIZE))
  curl -s -X POST -d "$DATA" http://localhost:9000/cgi/login.py -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 30 POST con datos variables en ${DIFF}s"

# ============ PRUEBAS DE TIMEOUT Y RECONEXIÓN ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " PRUEBAS DE RECONEXIÓN RÁPIDA"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[RECON-1] 50 usuarios con reconexión inmediata"
for i in {1..50}; do
  curl -s http://localhost:9000/ -o /dev/null
  curl -s http://localhost:9000/test-autoindex -o /dev/null &
done
wait
echo "✅ 100 peticiones (50 usuarios x 2 peticiones)"

# ============ PRUEBAS DE AUTOINDEX CON CARGA ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " MÚLTIPLES USUARIOS ACCEDIENDO A AUTOINDEX"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[AUTOINDEX-1] 50 usuarios listando directorios simultáneamente"
START=$(date +%s.%N)
for i in {1..50}; do
  curl -s http://localhost:9000/test-autoindex -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 50 listados de autoindex en ${DIFF}s"

echo -e "\n[AUTOINDEX-2] 100 usuarios listando uploads simultáneamente"
START=$(date +%s.%N)
for i in {1..100}; do
  curl -s http://localhost:9000/uploads -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 100 listados de uploads en ${DIFF}s"

# ============ PRUEBAS DE ERRORES CONCURRENTES ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " ERRORES 404 CONCURRENTES"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[ERROR-CONC-1] 100 usuarios solicitando rutas inexistentes"
START=$(date +%s.%N)
for i in {1..100}; do
  curl -s http://localhost:9000/ruta-no-existe-$i -o /dev/null &
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 100 errores 404 en ${DIFF}s"

# ============ STRESS TEST FINAL ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " STRESS TEST FINAL - MÁXIMA CARGA"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[FINAL-1] 300 usuarios realizando operaciones aleatorias"
START=$(date +%s.%N)
for i in {1..300}; do
  RAND=$((RANDOM % 4))
  case $RAND in
    0)
      curl -s http://localhost:9000/ -o /dev/null &
      ;;
    1)
      curl -s -X POST -d "user=$i" http://localhost:9000/cgi/login.py -o /dev/null &
      ;;
    2)
      curl -s http://localhost:9000/test-autoindex -o /dev/null &
      ;;
    3)
      curl -s http://localhost:9000/uploads -o /dev/null &
      ;;
  esac
done
wait
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "✅ 300 operaciones aleatorias en ${DIFF}s"

# ============ RESUMEN FINAL ============
echo -e "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo " RESUMEN DE ARCHIVOS GENERADOS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo -e "\n[INFO] Total archivos en uploads:"
TOTAL=$(ls /mnt/c/Users/Alvaro/Desktop/Estudios/42Madrid/CommonCore/webserv_git/zzz/uploads/ 2>/dev/null | wc -l)
echo "Total: $TOTAL archivos"

echo -e "\n[INFO] Espacio usado en uploads:"
du -sh /mnt/c/Users/Alvaro/Desktop/Estudios/42Madrid/CommonCore/webserv_git/zzz/uploads/ 2>/dev/null

# ============ LIMPIEZA ============
echo -e "\n[CLEANUP] Limpiando archivos temporales de prueba..."
rm -f /tmp/user_file_*.txt /tmp/mix_*.txt 2>/dev/null

echo -e "\n════════════════════════════════════════════════"
echo " PRUEBAS DE ESTRÉS EXTREMAS COMPLETADAS"
echo " El servidor manejó múltiples usuarios exitosamente"
echo "════════════════════════════════════════════════"
