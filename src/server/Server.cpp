#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"
#include <cstring>

Server::Server(std::vector<ServerConfig>& server)
{
	this->_servers = server;
	initSockets();
}

void Server::initSockets()
{
	std::string					fullhost;
	int							fdSocket;
	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		fullhost = _servers[i].getHost() + ":" + std::to_string(_servers[i].getPort());
		if (fdSocket = socket(AF_INET, SOCK_STREAM, 0) < 0)
		{
			std::cerr << "socket() failed for " << fullhost << ": " << strerror(errno) << std::endl;
			continue;
		}
		this->_listeners[fullhost] = fdSocket;
	}
}