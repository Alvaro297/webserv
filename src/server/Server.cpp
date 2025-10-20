#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"
#include "../../inc/Client.hpp"


Server::Server(std::vector<ServerConfig>& server)
{
	this->_servers = server;
	initSockets();
}

Server::Server() {}

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		this->_listeners = other._listeners;
		this->_servers = other._servers;
	}
	return *this;
}

Server::~Server() {}

void Server::createListener(const ServerConfig& config, const std::string& fullhost)
{
	Listeners	listener;
	int			fdSocket;

	fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (fdSocket < 0)
	{
		std::cerr << "socket() failed for " << fullhost << ": " << strerror(errno) << std::endl;
		return;
	}
	fcntl(fdSocket, F_SETFL, O_NONBLOCK);
	listener.connect.sin_family = AF_INET;
	listener.connect.sin_port = htons(config.getPort());
	listener.connect.sin_addr.s_addr = inet_addr(config.getHost().c_str());
	std::memset(listener.connect.sin_zero, 0, sizeof(listener.connect.sin_zero));
	if (bind(fdSocket, (struct sockaddr*)&listener.connect, sizeof(listener.connect)) < 0)
	{
		std::cerr << "bind() failed for " << fullhost << ": " << strerror(errno) << std::endl;
		close(fdSocket);
		return;
	}
	if (listen(fdSocket, SOMAXCONN) < 0)
	{
		std::cerr << "listen() failed for " << fullhost << ": " << strerror(errno) << std::endl;
		close(fdSocket);
		return;
	}
	listener.fd = fdSocket;
	listener.servers.push_back(config);
	this->_listeners[fullhost] = listener;
}

void Server::initSockets()
{
	std::string		fullhost;

	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		// Convert port to string (C++98 compatible)
		std::stringstream ss;
		ss << _servers[i].getPort();
		fullhost = _servers[i].getHost() + ":" + ss.str();
		if (this->_listeners.find(fullhost) != this->_listeners.end())
		{
			this->_listeners[fullhost].servers.push_back(_servers[i]);
			continue;
		}
		createListener(_servers[i], fullhost);
	}
}

static bool lineFinish(std::string& line)
{
	bool isLineFinish = false, isCr = false;
	std::string newLine = "";
	char c;
	for (size_t i = 0; i < line.size(); ++i)
	{
		c = line[i];
		if (c == 13)
		{
			isCr = true;
			continue;
		}
		else if (c == 10 && isCr)
		{
			isLineFinish = true;
			break;
		}
		isCr = false;
		newLine.push_back(c);
	}
	line = newLine;
	return isLineFinish;
}

void Server::readClient(int fds)
{
	std::vector<char> buffer(4096);
	int bytes = recv(fds, buffer.data(), buffer.size(), 0);
	std::string line;
	if (bytes > 0)
	{
		line = std::string(buffer.data(), bytes);
		
		if (!lineFinish(line))
		{
			this->_client[fds].appendReadBuffer(line);
			readClient(fds);
		}
		//else
			//PersonaB->Dani
		//\r\n\r\n
		//Falta mirar el final de los Headers luego pasarselo a Dani
	}
	else if (bytes == 0)
	{
		this->_client[fds].~Client();
		this->_client.erase(fds);
	}
	else
		throw ConnexionException();
	std::cout << bytes;
}


void Server::acceptSocket(int fds)
{
	int connect = 0;

	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	connect = accept(fds, (struct sockaddr*)&client_addr, &client_len);
	if (connect < 0)
	{
		std::cerr << "accept() failed: " << strerror(errno) << std::endl;
		return ;
	}
	fcntl(connect, F_SETFL, O_NONBLOCK);
	this->_client[connect] = Client(connect);
	std::cout << "Activity detected on fd: " << fds << std::endl;
}

bool isListener(std::map<std::string, Listeners> _listeners, int fdListener)
{
	for (std::map<std::string, Listeners>::iterator it = _listeners.begin();
			 it != _listeners.end(); ++it)
	{
		if (it->second.fd == fdListener)
			return true;
	}
	return false;
}

void Server::run()
{
	int ready, nbrPoll = 0;
	struct pollfd tmp;

	//Loop principal
	while (true)
	{
		std::vector<struct pollfd> fds;
		fds.clear();
		for (std::map<std::string, Listeners>::iterator it = _listeners.begin();
			 it != _listeners.end(); ++it)
		{
			tmp.fd = it->second.fd;
			tmp.events = POLLIN;
			tmp.revents = 0;
			nbrPoll++;
			fds.push_back(tmp);
		}
		for (std::map<int,Client>::iterator it = _client.begin(); it != _client.end(); ++it)
		{
			struct pollfd pfd;
			pfd.fd = it->first;
			pfd.events = POLLIN;
			pfd.revents = 0;
			nbrPoll++;
			fds.push_back(pfd);
		}
		if (fds.empty())
		{
			std::cerr << "No file descriptors to poll." << std::endl;
			break;
		}
		ready = poll(&fds[0], fds.size(), -1);
		if (ready < 0)
		{
			std::cerr << "poll() failed: " << strerror(errno) << std::endl;
			break;
		}
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (isListener(this->_listeners, fds[i].fd))
					acceptSocket(fds[i].fd);
				else
					readClient(fds[i].fd);
			}
		}
	}
}
