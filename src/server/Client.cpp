#include "../../inc/Client.hpp"

Client::Client() : fd(-1), readBuffer(""), writeBuffer("")
{
}

Client::Client(int client_fd) : fd(client_fd), readBuffer(""), writeBuffer("")
{
	std::cout << "Client created with fd: " << fd << std::endl;
}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		this->fd = other.fd;
		this->readBuffer = other.readBuffer;
		this->writeBuffer = other.writeBuffer;
	}
	return *this;
}

Client::~Client()
{
	if (fd >= 0)
	{
		std::cout << "Closing client fd: " << fd << std::endl;
		close(fd);
		fd = -1;
	}
}

int Client::getFd() const
{
	return fd;
}

const std::string& Client::getReadBuffer() const
{
	return readBuffer;
}

const std::string& Client::getWriteBuffer() const
{
	return writeBuffer;
}

void Client::appendReadBuffer(const std::string& data)
{
	readBuffer.append(data);
}

void Client::appendWriteBuffer(const std::string& data)
{
	writeBuffer.append(data);
}