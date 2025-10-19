#ifndef SERVER_HPP
# define SERVER_HPP
# include "ServerConfig.hpp"
# include <iostream>
# include <vector>
# include <map>
# include <cstring>
# include <unistd.h>
# include <cerrno>
# include <sstream>
# include <fcntl.h>
# include "Client.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netinet/in.h> // For sockaddr_in
# include <poll.h>


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
	std::map<int, Client> _client;

	void initSockets(); //Inicializar sockets (Rellena _listeners) -Falta
	void closeSockets(); //Cierra los sockets (No todos) -Falta
	void createListener(const ServerConfig& config, const std::string& fullhost);
	void acceptSocket(int fd);
	void readClient(int fd);
public:
	Server();
	Server(std::vector<ServerConfig>& servers);
	Server& operator=(const Server& other);
	~Server();

	void run(); //Escuchar (Bucle infinito) -Falta
	void stop(); //Parar la escucha -Falta

	class ConnexionException : public std::exception
	{
		public:
			const char* what() const throw() { return "Error in the conexion client server"; };
	};
};


#endif