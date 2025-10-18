#include "../../inc/Client.hpp"

// Constructor por defecto
Client::Client() : fd(-1), readBuffer(""), writeBuffer("")
{
}

// Constructor con fd
Client::Client(int client_fd) : fd(client_fd), readBuffer(""), writeBuffer("")
{
	std::cout << "Client created with fd: " << fd << std::endl;
}

// Operador de asignación
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

// Destructor
Client::~Client()
{
	if (fd >= 0)
	{
		std::cout << "Closing client fd: " << fd << std::endl;
		close(fd);
		fd = -1;
	}
}

// Getters
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