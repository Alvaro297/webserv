#ifndef CLIENT_HPP
# define CLIENT_HPP

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

class Client
{
private:
	int fd;
	std::string readBuffer;
	std::string writeBuffer;
public:
	Client();
	Client(int fd);
	Client& operator=(const Client& other);
	~Client();

	// Getters
	int getFd() const;
	const std::string& getReadBuffer() const;
	const std::string& getWriteBuffer() const;

	// Setters
	void appendReadBuffer(const std::string& data);
	void appendWriteBuffer(const std::string& data);
};

#endif