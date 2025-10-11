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

	void initSockets();
	void closeSockets();
public:
	Server();
	Server(std::vector<ServerConfig>& servers);
	~Server();

	void run();
	void stop();
};


#endif