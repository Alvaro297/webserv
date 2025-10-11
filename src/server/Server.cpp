#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"

Server::Server(const std::vector<ServerConfig>& server)
{
	this->_servers = server;
	initSockets();
}

void Server::initSockets()
{
	
}