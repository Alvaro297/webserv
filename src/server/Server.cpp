#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Response.hpp"
#include "../../inc/Handler.hpp"

static volatile sig_atomic_t g_shutdown = 0;

Server::Server(std::vector<ServerConfig>& server)
{
	this->_servers = server;
	initSockets();
}

Server::Server() {}

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		this->_listeners = other._listeners;
		this->_servers = other._servers;
	}
	return *this;
}

Server::~Server() {}


static std::string intToString(const int port)
{
	std::stringstream ss;
	std::string result;
	
	ss << port;
	result = ss.str();
	return result;
}

std::string generateErrorPage(const int errorCode, const std::string bodyError)
{
	std::ostringstream page;
	
	page << "<html>\n"
		<<"<head><title>" << errorCode << " " << bodyError << "</title></head>\n"
		<< "<body>"
		<<"<center><h1>" << errorCode << " " << bodyError << "</center></h1>"
		<<"</body>\n</html>";
	return page.str();
}

ServerConfig* Server::extractFullPath(std::string fullBuffer)
{
	int port;
	std::string host, fullPath, path;
	size_t posOfLine = fullBuffer.find("Host:") + 5;
	size_t end = fullBuffer.find("\r\n", posOfLine);

	if (end != std::string::npos)
	{
		std::string line = fullBuffer.substr(posOfLine, end - posOfLine);
		line.erase(0, line.find_first_not_of(" \t"));
		size_t dotsPosition = line.find(":");
		if (dotsPosition != std::string::npos)
		{
			host = line.substr(0, dotsPosition);
			if (host == "localhost")
				host = "127.0.0.1";
			port = std::atoi(line.substr(dotsPosition + 1).c_str()); 
		}
		else
		{
			host = "127.0.0.1";
			port = 80;
		}
	}
	std::stringstream ss;
	ss << port;
	std::string fullhost = host + ":" + ss.str();
	std::map<std::string, Listeners>::iterator it = _listeners.find(fullhost);
	if (it != _listeners.end() && !it->second.servers.empty())
		return &it->second.servers[0];
	
	// If not found, try with 0.0.0.0 (fallback for localhost/127.0.0.1)
	// Esto es si lo estas ejecutando en wsl sino comentarlo
	if (host == "127.0.0.1")
	{
		std::string fallbackHost = "0.0.0.0:" + ss.str();
		it = _listeners.find(fallbackHost);
		if (it != _listeners.end() && !it->second.servers.empty())
			return &it->second.servers[0];
	}
	
	return NULL;
}


void Server::processRequest(int fd, const std::string& fullBuffer)
{
	std::cout << "[DEBUG] processRequest called with fd=" << fd << std::endl;
	std::string fullPath;
	Request req = Request();
	ServerConfig* config = extractFullPath(fullBuffer);
	std::cout << "[DEBUG] extractFullPath returned: " << (config ? "valid config" : "NULL") << std::endl;

	if (config)
		fullPath = config->getRoot();
	else
	{
		this->_client[fd].appendWriteBuffer(generateErrorPage(400, "Config not found"));
		return ;
	}
	//MARIO
	if (req.parseRequestValidity(fullBuffer))
	{
		// First, check for a matching location return (longest prefix match)
		if (config) {
			const std::vector<LocationConfigStruct>& locs = config->getLocations();
			const LocationConfigStruct* bestLoc = NULL;
			size_t bestLen = 0;
			std::string reqPath = req.getPath();
			for (size_t i = 0; i < locs.size(); ++i) {
				const LocationConfigStruct& loc = locs[i];
				if (reqPath.compare(0, loc.path.length(), loc.path) == 0 && loc.path.length() > bestLen) {
					bestLen = loc.path.length();
					bestLoc = &loc;
				}
			}
			if (bestLoc && bestLoc->return_code != 0) {
				Response r;
				int code = bestLoc->return_code;
				std::string msg = (code == 301) ? "Moved Permanently" : (code == 302) ? "Found" : "Redirect";
				r.setStatus(code, msg);
				std::string locurl = bestLoc->return_url;
				if (locurl.empty()) locurl = "/";
				r.setHeader("Location", locurl);
				r.setHeader("Content-Type", "text/html");
				r.setBody(std::string("<html><body><h1>") + intToString(code) + " " + msg + "</h1></body></html>");
				this->_client[fd].appendWriteBuffer(r.genResponseString());
				return;
			}
		}

		// If not a return redirection, check for CGI by extension
		// MARIO
		size_t extensionDot = req.getPath().find('.');
		std::string extension;
		if (extensionDot != std::string::npos) //MARIO
			extension = req.getPath().substr(extensionDot); // MARIO
		if (config && !extension.empty() && config->supportsExtension(extension)) // MARIO
		{
			CGIHandler cgiHandler(config->getCgiExtensions(), fullPath, config->getServerName(), intToString(config->getPort()));
			Response resp = cgiHandler.handle(req);
			this->_client[fd].appendWriteBuffer(resp.genResponseString());
			return;
		}
	}
	else
	{
		this->_client[fd].appendWriteBuffer(generateErrorPage(400, "Bad Request"));
		return;
	}
	Handler hand(fullPath, *config);
	Response resp = hand.handleRequest(fullBuffer);
	if (resp.getError() != 0)
		this->_client[fd].appendWriteBuffer(generateErrorPage(resp.getError(), resp.getBody()));
	else
		this->_client[fd].appendWriteBuffer(resp.genResponseString());
}

void Server::createListener(const ServerConfig& config, const std::string& fullhost)
{
	Listeners	listener;
	int			fdSocket;

	fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (fdSocket < 0)
	{
		std::cerr << "socket() failed for " << fullhost << ": " << strerror(errno) << std::endl;
		return;
	}
	
	// Permitir reutilizar el puerto inmediatamente después de cerrar el servidor
	int opt = 1;
	if (setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "setsockopt(SO_REUSEADDR) failed: " << strerror(errno) << std::endl;
	}
	
	fcntl(fdSocket, F_SETFL, O_NONBLOCK);
	listener.connect.sin_family = AF_INET;
	listener.connect.sin_port = htons(config.getPort());
	listener.connect.sin_addr.s_addr = inet_addr(config.getHost().c_str());
	std::memset(listener.connect.sin_zero, 0, sizeof(listener.connect.sin_zero));
	if (bind(fdSocket, (struct sockaddr*)&listener.connect, sizeof(listener.connect)) < 0)
	{
		std::cerr << "bind() failed for " << fullhost << ": " << strerror(errno) << std::endl;
		close(fdSocket);
		return;
	}
	if (listen(fdSocket, SOMAXCONN) < 0)
	{
		std::cerr << "listen() failed for " << fullhost << ": " << strerror(errno) << std::endl;
		close(fdSocket);
		return;
	}
	listener.fd = fdSocket;
	listener.servers.push_back(config);
	this->_listeners[fullhost] = listener;
}

void Server::initSockets()
{
	std::string		fullhost;

	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		std::stringstream ss;
		ss << _servers[i].getPort();
		fullhost = _servers[i].getHost() + ":" + ss.str();
		if (this->_listeners.find(fullhost) != this->_listeners.end())
		{
			this->_listeners[fullhost].servers.push_back(_servers[i]);
			continue;
		}
		createListener(_servers[i], fullhost);
	}
}

static std::string whatMethod(std::string line)
{
	std::string typeMethod;
	size_t firstSpace = line.find(" ");

	typeMethod = line.substr(0, firstSpace);
	if (typeMethod != "POST" && typeMethod != "GET"
		&& typeMethod != "DELETE" && typeMethod != "FETCH" && typeMethod != "HEAD")
		return "GET";
	return typeMethod;
}

static bool lineFinish(std::string line)
{
	std::string eof = "\r\n\r\n";
	std::string contLength = "Content-Length:";
	size_t bodyLength = 0;
	std::string typeMethod = whatMethod(line);

	size_t posOfLength = line.find(contLength);
	if (posOfLength != std::string::npos)
	{
		size_t start = posOfLength + contLength.length();
		size_t end = line.find("\r\n", start);
		if (end != std::string::npos)
		{
			std::string lengthStr = line.substr(start, end - start);
			lengthStr.erase(0, lengthStr.find_first_not_of(" \t"));
			lengthStr.erase(lengthStr.find_last_not_of(" \t") + 1);
			bodyLength = std::atol(lengthStr.c_str());
		}
	}
	if (line.find(eof) != std::string::npos)
	{
		std::cout << "[DEBUG] Found eof in line" << std::endl;
		if (typeMethod == "GET" || (typeMethod == "DELETE"
				&& posOfLength == std::string::npos))
			return true;
		std::string body = line.substr(line.find(eof) + eof.length());
		bool result = body.size() >= bodyLength;
		return result;
	}
	return false;
}

size_t getMaxBodySizeLocation(ServerConfig *config, std::string currentBuffer)
{
	size_t firstSpace = currentBuffer.find(" ");
	size_t secondSpace = currentBuffer.find(" ", firstSpace + 1);
	std::string path = currentBuffer.substr(firstSpace + 1, secondSpace - firstSpace - 1);
	size_t bodySize = config->getClientMaxBodySize();
	std::string pathLocation;
	size_t longPath = 0;

	for (size_t i = 0; i < config->getLocations().size(); i++)
	{
		pathLocation = config->getLocations()[i].path;
		if (path.substr(0, pathLocation.length()) == pathLocation && longPath < pathLocation.length())
		{
			size_t locMax = config->getLocations()[i].client_max_body_size;
			if (locMax > 0)
				bodySize = locMax;
			longPath = pathLocation.length();
		}
	}
	return bodySize;
}

void Server::readClient(int fds)
{
	std::string currentBuffer = this->_client[fds].getReadBuffer();
	size_t maxBodySize = 4194304;

	if (currentBuffer.find("Host:") != std::string::npos)
	{
		ServerConfig* config = extractFullPath(currentBuffer);
		if (config != NULL)
			maxBodySize = getMaxBodySizeLocation(config, currentBuffer);
	}

	if (this->_client[fds].getReadBuffer().size() > maxBodySize)
	{
		std::string response = generateErrorPage(413, "Payload Too Large");
		this->_client[fds].clearReadBuffer();
		this->_client[fds].appendWriteBuffer(response);
		closeClient(fds, "Request too large");
		return;
	}
	std::vector<char> buffer(4096);
	int bytes = recv(fds, buffer.data(), buffer.size(), 0);
	
	std::string line;
	if (bytes > 0)
	{
		std::string chunk(buffer.data(), bytes);
		this->_client[fds].appendReadBuffer(chunk);
		this->_client[fds].setLastActivity(time(NULL));
		line = this->_client[fds].getReadBuffer();
		if (lineFinish(line))
		{
			processRequest(fds, line);
			this->_client[fds].clearReadBuffer();
		}
	}
	else if (bytes == 0)
		closeClient(fds, "bytes is 0");
	else
	{
		if (errno != EAGAIN)
			closeClient(fds, "recv() failed");
	}
}

void Server::closeClient(int fd, const std::string& reason)
{
	std::cout << "[INFO] Cerrando cliente fd=" << fd << " - Motivo: " << reason << std::endl;
	close(fd);
	this->_client.erase(fd);
}

void Server::acceptSocket(int fds)
{
	int client_fd = 0;

	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	client_fd = accept(fds, (struct sockaddr*)&client_addr, &client_len);
	if (client_fd < 0)
	{
		std::cerr << "accept() failed: " << strerror(errno) << std::endl;
		return ;
	}
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	if (this->_client.size() >= 1000)
	{
		std::string response503 = generateErrorPage(503, "Service Unavailable");
		this->_client[client_fd].appendWriteBuffer(response503);
		send(client_fd, response503.c_str(), response503.size(), 0);
		close(client_fd);
		return;
	}
	// Insertar directamente en el map sin crear objeto por defecto
	this->_client.insert(std::make_pair(client_fd, Client(client_fd)));
}

static bool isListener(std::map<std::string, Listeners> _listeners, int fdListener)
{
	for (std::map<std::string, Listeners>::iterator it = _listeners.begin();
			 it != _listeners.end(); ++it)
	{
		if (it->second.fd == fdListener)
			return true;
	}
	return false;
}

static void signalHandler(int signum)
{
	(void)signum;
	g_shutdown = 1;
}

void Server::closeServer()
{
	for (std::map<std::string,Listeners>::iterator it = _listeners.begin(); it != _listeners.end();)
	{
		if (it->second.fd)
			close(it->second.fd);
		this->_listeners.erase(it++);
	}
	for (std::map<int,Client>::iterator it = _client.begin(); it != _client.end();)
	{
		if (it->first)
			close(it->first);
		this->_client.erase(it++);
	}
}

void Server::run()
{
	int ready;
	struct pollfd tmp;

	signal(SIGINT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
	//Falta controlar error 503 (Muchos clientes)
	//Loop principal
	while (!g_shutdown)
	{
		std::vector<struct pollfd> fds;
		fds.clear();
		for (std::map<std::string, Listeners>::iterator it = _listeners.begin();
			 it != _listeners.end(); ++it)
		{
			tmp.fd = it->second.fd;
			tmp.events = POLLIN;
			tmp.revents = 0;
			fds.push_back(tmp);
		}
		for (std::map<int,Client>::iterator it = _client.begin(); it != _client.end(); ++it)
		{
			struct pollfd pfd;
			pfd.fd = it->first;
			pfd.events = POLLIN;
			if (!it->second.getWriteBuffer().empty())
				pfd.events |= POLLOUT;
			pfd.revents = 0;
			fds.push_back(pfd);
		}
		if (fds.empty())
		{
			std::cerr << "No file descriptors to poll." << std::endl;
			break;
		}
		for (std::map<int,Client>::iterator it = _client.begin(); it != _client.end();)
		{
			time_t now = time(NULL);
			// Esto es el timeout modificarlo a vuestra medida para si quereis seguir o no (Actualmente en 12 sec)
			if ((now - it->second.getLastActivity()) > 1200)
			{
				std::string response408 = generateErrorPage(408, "Request Timeout");
				this->_client[it->first].appendWriteBuffer(response408);
				close(it->first);
				this->_client.erase(it++);
			}
			else
				++it;
		}
		//Este bucle se actualiza cada 1 sec gracias al poll por lo que el timeout como mucho sale con un sec de retraso
		ready = poll(&fds[0], fds.size(), 1000);
		if (ready < 0)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "poll() failed: " << strerror(errno) << std::endl;
			break;
		}
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLERR)
			{
				closeClient(fds[i].fd, "Socket error");
				continue;
			}
			if (fds[i].revents & POLLNVAL)
			{
				std::cerr << "[ERROR] Invalid fd=" << fds[i].fd << std::endl;
				_client.erase(fds[i].fd);
				continue;
			}
			if (fds[i].revents & POLLIN)
			{
				if (isListener(this->_listeners, fds[i].fd))
					acceptSocket(fds[i].fd);
				else
					readClient(fds[i].fd);
			}
			if (fds[i].revents & POLLOUT)
			{
				bool success = this->_client[fds[i].fd].writeClient();
				if (!success)
					closeClient(fds[i].fd, "Error writing to client");
			}
			if (fds[i].revents & POLLHUP)
				closeClient(fds[i].fd, "Client hung up");
		}
	}
	closeServer();
}
