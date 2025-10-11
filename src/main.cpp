#include "../inc/configParser.hpp"
#include "../inc/ServerConfig.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file.conf>\n";
		return 1;
	}
	//if (!ConfigParser::RunParser(argv[1]))
	//	return 1;
	ServerConfig(ConfigParser::RunParser(argv[1]));
	return 0;
}
