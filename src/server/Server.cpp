#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"
#include "../../inc/Client.hpp"
#include "Response.hpp"
#include "Handler.hpp"

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

void Server::processRequest(int fd, const std::string& fullBuffer)
{
	Handler hand("default_root");
	Response resp = hand.handleRequest(fullBuffer);
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

static bool lineFinish(std::string line)
{
	std::string eof = "\r\n\r\n";

	if (line.find(eof) != std::string::npos)
		return true;
	return false;
}

void Server::readClient(int fds)
{
	if (this->_client[fds].getReadBuffer().size() > 4194304)
	{
		std::string response = 
			"HTTP/1.1 413 Payload Too Large\r\n"
			"Content-Length: 0\r\n"
			"Connection: close\r\n\r\n";
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
				std::cout << "Timeout of conexion 408" << std::endl; //Para Dani
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
