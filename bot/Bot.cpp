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
	int sockfd;
	struct sockaddr_in serv_addr;

	// Create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return ;
	}

	// Set server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port_); // Common IRC port
	inet_pton(AF_INET, server_addr_.c_str(), &(serv_addr.sin_addr));

	// Connect to the server
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{
		std::cerr << "Error connecting to server" << std::endl;
		return;
	}
	//needs to config the nickname, username and server password and send it to the server;
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