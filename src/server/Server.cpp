#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"


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
	std::string					fullhost;

	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		// Convert port to string (C++98 compatible)
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

void Server::acceptSocket(int nbrPoll, int connect, pollfd *fds)
{
	for (int i = 0; i < nbrPoll; i++)
	{
		std::cout << "Estamos dentro de socketAccepted " << nbrPoll << std::endl;
		if (fds[i].revents & POLLIN)
		{
			std::cout << "Estamos dentro de socketAccepted del if " << nbrPoll << std::endl;
			sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			connect = accept(fds[i].fd, (struct sockaddr*)&client_addr, &client_len);
			if (connect < 0)
			{
				std::cerr << "accept() failed: " << strerror(errno) << std::endl;
				continue;
			}
			std::cout << "Activity detected on fd: " << fds[i].fd << std::endl;
		}
	}
}

void Server::run()
{
	int nbrPoll = 0, connect = 0, ready;
	struct pollfd fds[this->_listeners.size()];
	
	for (std::map<std::string, Listeners>::iterator it = _listeners.begin(); 
		 it != _listeners.end(); ++it)
	{
		fds[nbrPoll].fd = it->second.fd;
		fds[nbrPoll].events = POLLIN;
		fds[nbrPoll].revents = 0;
		nbrPoll++;
	}
	//Loop principal
	while (true)
	{
		ready = poll(fds, nbrPoll, -1);
		if (ready < 0)
		{
			std::cerr << "poll() failed: " << strerror(errno) << std::endl;
			break;
		}
		acceptSocket(nbrPoll, connect, fds);
	}
}
