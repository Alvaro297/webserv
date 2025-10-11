#ifndef SERVER_HPP
# define SERVER_HPP
# include "ServerConfig.hpp"
# include <iostream>
# include <vector>
# include <map>

class Server
{
private:
	std::vector<ServerConfig>	_servers;
	std::map<std::string,int>	_listeners;

	void initSockets(); //Inicializar sockets (Rellena _listeners) -Falta
	void closeSockets(); //Cierra los sockets (No todos) -Falta
public:
	Server();
	Server(std::vector<ServerConfig>& servers);
	~Server();

	void run(); //Escuchar (Bucle infinito) -Falta
	void stop(); //Parar la escucha -Falta
};


#endif