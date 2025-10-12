#ifndef SERVER_HPP
# define SERVER_HPP
# include "ServerConfig.hpp"
# include <iostream>
# include <vector>
# include <map>

struct Listeners
{
	int fd;
	sockaddr_in connect;
	std::vector<ServerConfig> servers;
};


class Server
{
private:
	std::vector<ServerConfig>	_servers;
	std::map<std::string,Listeners>	_listeners; //Host:puerto y listener fd

	void initSockets(); //Inicializar sockets (Rellena _listeners) -Falta
	void closeSockets(); //Cierra los sockets (No todos) -Falta
	void Server::createListener(const ServerConfig& config, const std::string& fullhost);
public:
	Server();
	Server(std::vector<ServerConfig>& servers);
	~Server();

	void run(); //Escuchar (Bucle infinito) -Falta
	void stop(); //Parar la escucha -Falta
};


#endif