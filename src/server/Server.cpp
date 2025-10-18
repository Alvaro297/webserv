#include "../../inc/Server.hpp"
#include "../../inc/ServerConfig.hpp"


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

void Server::acceptSocket(int nbrPoll, int connect, std::vector<struct pollfd> fds)
{
	for (int i = 0; i < nbrPoll && i < static_cast<int>(fds.size()); ++i)
	{
		if (fds[i].revents & POLLIN)
		{
			sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			connect = accept(fds[i].fd, (struct sockaddr*)&client_addr, &client_len);
			if (client_fd < 0)
			{
				std::cerr << "accept() failed: " << strerror(errno) << std::endl;
				continue;
			}
			fcntl(connect, F_SETFL, O_NONBLOCK);
			this->_client[connect] = Client(connect);
			std::cout << "Activity detected on fd: " << fds[i].fd << std::endl;
		}
	}
}

void Server::run()
{
	int connect = 0, ready;
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
			fds.push_back(tmp);
		}
		for (std::map<int,Client>::iterator it = _client.begin(); it != _client.end(); ++it)
		{
			struct pollfd pfd;
			pfd.fd = it->first;
			pfd.events = POLLIN;
			pfd.revents = 0;
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
		acceptSocket(static_cast<int>(fds.size()), connect, fds);
	}
}
