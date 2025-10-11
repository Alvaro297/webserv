#ifndef ServerConfig_HPP
# define ServerConfig_HPP

# include <configParser.hpp>
# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <winsock2.h>
# include <sys/socket.h>


class ServerConfig
{
private:
	int							id;
	int							port;
	std::string					host;
	std::string					server_name;
	std::string					root;
	int							ipv;
	std::vector<std::string>	index;
	int findTypeOfIpv(std::string host);
public:
	ServerConfig();
	ServerConfig(const ServerConfigStruct server);
	ServerConfig(const ServerConfig& other);
	ServerConfig& operator=(const ServerConfig& other);
	~ServerConfig();



	//Getters
	int getId() const;
	int getPort() const;
	const std::string& getHost() const;
	const std::string& getServerName() const;
	const std::string& getRoot() const;
	int getIpv() const;
	const std::vector<std::string>& getIndex() const;

	//Setters
	void setId(int value);
	void setPort(int value);
	void setHost(const std::string& value);
	void setServerName(const std::string& value);
	void setRoot(const std::string& value);
	void setIpv(const int ipv);
	void setIndex(const std::vector<std::string> index);

};



#endif