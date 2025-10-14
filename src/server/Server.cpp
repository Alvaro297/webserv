#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

Server::Server(std::vector<ServerConfig>& server)
{
	this->_servers = server;
	initSockets();
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

/*void Server::run()
{
	
}*/