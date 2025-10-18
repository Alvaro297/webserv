#ifndef CLIENT_HPP
# define CLIENT_HPP


class Client
{
private:
	int fd;
	std::string readBuffer;
	std::string writeBuffer;
	time_t lastActivity;
public:
	Client();
	Client(int fd);
	Client& operator=(const Client& other);
	~Client();
};

#endif