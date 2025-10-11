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
public:
	Server();
	Server(std::vector<ServerConfig> servers);
	~Server();
};


#endif