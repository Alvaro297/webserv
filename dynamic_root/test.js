function log(msg) { document.getElementById('log').textContent = msg; }

function createFile() {
  fetch('/dynamic_root/test-file.txt', { method: 'POST', body: 'Contenido creado por AJAX' })
    .then(res => res.text())
    .then(txt => log('POST -> ' + resStatus(res) + '\n' + txt))
    .catch(err => log('Error: ' + err));
}

function deleteFile() {
  fetch('/dynamic_root/test-file.txt', { method: 'DELETE' })
    .then(res => res.text().then(t => log('DELETE -> ' + resStatus(res) + '\n' + t)))
    .catch(err => log('Error: ' + err));
}

function fetchCGI() {
  fetch('/dynamic_root/cgi-bin/hello.py')
    .then(res => res.text())
    .then(txt => log('CGI -> OK\n' + txt))
    .catch(err => log('CGI Error: ' + err));
}

function resStatus(res) { return res.status + ' ' + res.statusText; }
