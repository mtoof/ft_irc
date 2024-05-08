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
	if (Bot::signal_)
		return;
	createBotSocket();
	poll_fd_.fd = server_fd_;
    poll_fd_.events = POLLIN | POLL_OUT;
    poll_fd_.revents = 0;
	int event;
	while (!Bot::signal_)
	{
		event = poll(&poll_fd_, POLL_IN, -1);
		if (event == -1 && !Bot::signal_)
			throw std::runtime_error("Error poll");
		if (poll_fd_.revents & (POLL_IN | POLL_OUT))
		{
			readBuffer();
		}
	}
	close(poll_fd_.fd);
	close(server_fd_);
}

void Bot::createBotSocket()
{
	memset(&addr_info_, 0, sizeof addr_info_);
	addr_info_.ai_family = AF_UNSPEC;
	addr_info_.ai_socktype = SOCK_STREAM;
	// first, load up address structs with getaddrinfo():
	getaddrinfo(server_addr_.c_str(), std::to_string(server_port_).c_str(), &addr_info_, &serv_addr_info_);
	server_fd_ = socket(serv_addr_info_->ai_family, serv_addr_info_->ai_socktype, serv_addr_info_->ai_protocol);
	if (server_fd_ < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return ;
	}
	std::cout << "Bot created a socket successfuly" << std::endl;
	int flags = fcntl(server_fd_, F_GETFL, 0);
    if (flags < 0)
	{
		close(server_fd_); // Close the socket if fcntl fails
		throw std::runtime_error("Error getting socket flags");
	}
    if (fcntl(server_fd_, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		close(server_fd_); // Close the socket if fcntl fails
		throw std::runtime_error("Error setting socket to NON-BLOCKING");
	}
    // Connect to server
	if (connect(server_fd_, serv_addr_info_->ai_addr, serv_addr_info_->ai_addrlen) < 0)
	{
		if (errno != EINPROGRESS) { // Check if connection is in progress
			std::cerr << "Error connecting to server" << std::endl;
			close(server_fd_); // Close the socket if connect fails
			return;
		}
	}
	std::cout << "Bot connected to server successfuly" << std::endl;
	free(serv_addr_info_);
	sendInfo();
}

void Bot::sendInfo()
{
	if (info_file_.empty())
	throw std::runtime_error("Need a config file");
	std::ifstream info_file(info_file_);
	readFile(info_file);
	std::string line;
	line.clear();
	while (std::getline(info_file, line))
	{
		line += "\r\n";
		if (send(server_fd_, line.c_str(), line.length(), 0) < 0)
		{
			std::cout << RED "Couldn't send data, Check the connection please!" RESET << std::endl;
			info_file.clear();
			info_file.seekg(0, info_file.beg);
			info_file.close();
			close(server_fd_);
			reConnection();
		}
		
	}
	info_file.clear();
	info_file.seekg(0, info_file.beg);
	info_file.close();
}

void Bot::readFile(std::ifstream &info_file)
{
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
	if (info_file.eof())
	{
		throw std::runtime_error("Config file is empty");	
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

std::string const	&Bot::getInfoFile() const
{
	return info_file_;
}

void Bot::reConnection()
{
	int counter = 10;
	while (counter && !Bot::signal_)
	{
		std::cout << '\r' << std::setw(2) << std::setfill('0') << counter-- << std::flush;
		sleep(1);
	}
	std::cout << "\nRetry again!!!\n" << std::flush;
	counter = 10;
	close(server_fd_);
	init_bot();
}