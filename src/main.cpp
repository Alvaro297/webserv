#include "../inc/configParser.hpp"
#include "../inc/ServerConfig.hpp"
#include "../inc/Server.hpp"
#include <iostream>


int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file.conf>\n";
		return 1;
	}
	
	std::vector<ServerConfig> servers = ConfigParser::RunParser(argv[1]);
	Server server(servers);
	server.run();
	for (size_t i = 0; i < servers.size(); ++i) {
		servers[i].printConfig();
		std::cout << std::endl;
	}
	return 0;
}
