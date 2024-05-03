#include "Bot.h"

bool Bot::signal_ = false;
Bot::Bot(std::string &server_address, int &port, std::string &password)
: server_addr_(server_address), server_port_(port), server_password_(password)
{
    init_bot();
}

Bot::~Bot()
{
}

void Bot::init_bot()
{
	createBotSocket();
	sendInfo();
	poll_fd_={bot_socket_, POLL_IN, 0};
	int event;
	while (!Bot::signal_)
	{
		event = poll(&poll_fd_, sizeof(poll_fd_), -1);
		if (event == -1 && !Bot::signal_)
			throw std::runtime_error("Error poll");
		if (poll_fd_.revents && POLL_IN)
		{
		}
	}
}

void Bot::createBotSocket()
{
	struct sockaddr_in serv_addr;

	// Create a socket
	bot_socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (bot_socket_ < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return ;
	}
	std::cout << "Bot created a socket successfuly" << std::endl;

	// Set server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port_); // Common IRC port
	inet_pton(AF_INET, server_addr_.c_str(), &(serv_addr.sin_addr));
	if (connect(bot_socket_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cerr << "Error connecting to server" << std::endl;
		return;
	}
}

void Bot::sendInfo()
{
	std::ifstream info_file(info_file_);
	if (!info_file.is_open())
	{
		throw std::runtime_error("Error opening the file!");
	}
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