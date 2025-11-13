#include "../../inc/Client.hpp"

Client::Client() : fd(-1), readBuffer(""), writeBuffer(""), lastActivity(time(NULL)), shouldClose(0) {}

Client::Client(int client_fd) : fd(client_fd), readBuffer(""), writeBuffer(""), lastActivity(time(NULL)), shouldClose(0)
{
	std::cout << "Client created with fd: " << fd << std::endl;
}

Client::Client(const Client& other) : fd(other.fd), readBuffer(other.readBuffer), writeBuffer(other.writeBuffer), lastActivity(other.lastActivity), shouldClose(other.shouldClose)
{
	std::cout << "Client copy constructor (fd: " << fd << ")" << std::endl;
}

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		this->fd = other.fd;
		this->readBuffer = other.readBuffer;
		this->writeBuffer = other.writeBuffer;
		this->lastActivity = other.lastActivity;
		this->shouldClose = other.shouldClose;
	}
	return *this;
}

Client::~Client()
{
}

int Client::getFd() const { return fd; }

const std::string& Client::getReadBuffer() const { return readBuffer; }

const std::string& Client::getWriteBuffer() const { return writeBuffer; }

const time_t& Client::getLastActivity() const { return lastActivity; }

int Client::getShouldClose() const { return shouldClose; }

void Client::appendReadBuffer(const std::string& data) { readBuffer.append(data); }

void Client::appendWriteBuffer(const std::string& data) { writeBuffer.append(data); }

void Client::setLastActivity(const time_t& time) { this->lastActivity = time; }

void Client::setShouldClose(const int shouldCloseChange) { this->shouldClose = shouldCloseChange; }

void Client::clearReadBuffer() { this->readBuffer.clear(); }

void Client::clearWriteBuffer() { this->writeBuffer.clear(); }

bool Client::writeClient()
{
	if (this->getWriteBuffer().empty())
		return true;
	else
	{
		this->setLastActivity(time(NULL));
		ssize_t bytes_send = send(this->fd, this->getWriteBuffer().c_str(), this->getWriteBuffer().length(), MSG_NOSIGNAL);
		if (bytes_send > 0)
			this->writeBuffer.erase(0, bytes_send);
		else if (bytes_send < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				std::cerr << "send() failed: " << strerror(errno) << std::endl;
				return false;
			}
		}
	}
	return true;
}