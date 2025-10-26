#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"
#include "../../inc/Client.hpp"


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

void Server::processRequest(int fd, const std::string& fullBuffer)
{
	(void) fullBuffer;
	std::string response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 13\r\n"
		"Connection: close\r\n"
		"\r\n"
		"Hello, World!";
	// TODO: Cuando Dani tenga el parser, llamarlo aquí
	// Request req = HTTPParser::parse(fullBuffer);
	// Response resp = handleRequest(req);
	// sendResponse(fd, resp);
	this->_client[fd].appendWriteBuffer(response);
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

static bool lineFinish(std::string line)
{
	std::string eof = "\r\n\r\n";

	if (line.find(eof) != std::string::npos)
		return true;
	return false;
}

void Server::readClient(int fds)
{
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
	{
		std::cout << "Cliente fd " << fds << " desconectado" << std::endl;
		close(fds);
		this->_client.erase(fds);
	}
	else  // bytes < 0
	{
		if (errno != EAGAIN)
		{
			std::cerr << "recv() failed: " << strerror(errno) << std::endl;
			close(fds);
			this->_client.erase(fds);
		}
	}
}


void Server::acceptSocket(int fds)
{
	int connect = 0;

	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	connect = accept(fds, (struct sockaddr*)&client_addr, &client_len);
	if (connect < 0)
	{
		std::cerr << "accept() failed: " << strerror(errno) << std::endl;
		return ;
	}
	fcntl(connect, F_SETFL, O_NONBLOCK);
	// Insertar directamente en el map sin crear objeto por defecto
	this->_client.insert(std::make_pair(connect, Client(connect)));
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

void Server::run()
{
	int ready;
	struct pollfd tmp;

	//Loop principal
	signal(SIGPIPE, SIG_IGN);
	while (true)
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
			if ((now - it->second.getLastActivity()) > 12)
			{
				std::cout << "Timeout of conexion" << std::endl;
				close(it->first);
				this->_client.erase(it++);
			}
			else
				++it;
		}
		ready = poll(&fds[0], fds.size(), 1000);
		if (ready < 0)
		{
			std::cerr << "poll() failed: " << strerror(errno) << std::endl;
			break;
		}
		for (size_t i = 0; i < fds.size(); i++)
		{
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
				{
					std::cerr << "Error writing to client " << fds[i].fd << std::endl;
					close(fds[i].fd);
					this->_client.erase(fds[i].fd);
				}
			}
		}
	}
}
