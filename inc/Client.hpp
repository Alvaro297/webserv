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
#include <ctime>
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
	time_t		lastActivity;
public:
	Client();
	Client(int fd);
	Client(const Client& other);  // Constructor de copia
	Client& operator=(const Client& other);
	~Client();

	// Getters
	int getFd() const;
	const std::string& getReadBuffer() const;
	const std::string& getWriteBuffer() const;
	const time_t& getLastActivity() const;

	// Setters
	void appendReadBuffer(const std::string& data);
	void appendWriteBuffer(const std::string& data);
	void setLastActivity(const time_t& time);
	void clearReadBuffer();
	void clearWriteBuffer();
	bool writeClient();
};

#endif