#include "Bot.h"

bool Bot::signal_ = false;
Bot::Bot(std::string &server_address, int &port, std::string &password, std::string const &filename)
: server_addr_(server_address), server_port_(port), server_password_(password), info_file_(filename)
{
	std::cout << RED "constructor called" RESET << std::endl;
}

Bot::~Bot()
{
	std::cout << RED "destructor called" RESET << std::endl;
}

void Bot::init_bot()
{
	createBotSocket();
	poll_fd_.fd = bot_socket_;
    poll_fd_.events = POLLIN;
    poll_fd_.revents = 0;
	int event;
	while (!Bot::signal_)
	{
		event = poll(&poll_fd_, POLL_IN, -1);
		if (event == -1 && !Bot::signal_)
			throw std::runtime_error("Error poll");
		if (poll_fd_.revents && POLL_IN)
		{
			readBuffer();
		}
	}
	close(poll_fd_.fd);
	close(bot_socket_);
}

void Bot::createBotSocket()
{
	// Create a socket
	bot_socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (bot_socket_ < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return ;
	}
	std::cout << "Bot created a socket successfuly" << std::endl;
	// Set server address
	server_socket_addr_.sin_family = AF_INET;
	server_socket_addr_.sin_port = htons(server_port_); // Common IRC port
	inet_pton(AF_INET, server_addr_.c_str(), &(server_socket_addr_.sin_addr));
	int flags = fcntl(bot_socket_, F_GETFL, 0);
    if (flags < 0) {
        close(bot_socket_); // Close the socket if fcntl fails
        throw std::runtime_error("Error getting socket flags");
    }
    if (fcntl(bot_socket_, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(bot_socket_); // Close the socket if fcntl fails
        throw std::runtime_error("Error setting socket to NON-BLOCKING");
    }
    // Connect to server
    if (connect(bot_socket_, (struct sockaddr *)&server_socket_addr_, sizeof(server_socket_addr_)) < 0) {
        if (errno != EINPROGRESS) { // Check if connection is in progress
            std::cerr << "Error connecting to server" << std::endl;
            close(bot_socket_); // Close the socket if connect fails
            return;
        }
    }
	std::cout << "Bot connected to server successfuly" << std::endl;
	sendInfo();
}

void Bot::sendInfo()
{
	std::cout << "info_file_ = " << getInfoFile() << std::endl;
	if (info_file_.empty())
		throw std::runtime_error("Need a config file");
	std::ifstream info_file(info_file_);
	if (!info_file.is_open())
	{
		throw std::runtime_error("Error opening the file!");
	}
	struct stat fileStat;
	if (stat(info_file_.c_str(), &fileStat) == 0)
	{
		if (S_ISDIR(fileStat.st_mode))
		{
			std::cout << info_file_;
			throw std::runtime_error(" Is a directory");
		}
	}
	std::string line;
	if (info_file.eof())
	{
		throw std::runtime_error("Config file is empty");	
	}
	line.clear();
	while (std::getline(info_file, line))
	{
		line += "\r\n";
		send(bot_socket_, line.c_str(), line.length(), 0);
	}
	info_file.clear();
	info_file.seekg(0, info_file.beg);
	info_file.close();
}

void Bot::signalhandler(int signum)
{
	switch (signum)
	{
		case SIGINT:
			std::cout << "SIGINT (Interrupt signal)" << std::endl;
			break;
		case SIGQUIT:
			std::cout << "SIGQUIT (Quit)" << std::endl;
			break;
	}
	signal_ = true;
}

std::string const &Bot::getServerAddr() const
{
    return this->server_addr_;
}

std::string const &Bot::getServerPassword() const
{
    return this->server_password_;
}

int const &Bot::getServerPort() const
{
    return this->server_port_;
}

std::string const	&Bot::getInfoFile() const
{
	return info_file_;
}