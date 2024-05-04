#include "Bot.h"

void Bot::readBuffer()
{
	std::cout << "before beginning of readbuffer info_file_ = " << getInfoFile() << std::endl;
	ssize_t readbyte = 0;
	std::vector<std::string> commands;
	char buffer[1024] = {};
	memset(buffer, 0, sizeof(buffer));
	readbyte = recv(bot_socket_, buffer, 1024 - 1, 0);
	if (readbyte < 0 && !Bot::signal_)
	{
		std::cerr << "recv function failed" << std::endl;
		return;
	}
	else if (!readbyte)
	{
		std::cout << RED << "Bot lost its connection with the server" << RESET << std::endl;
		int counter = 10;
		while (counter)
		{
			std::cout << '\r' << std::setw(2) << std::setfill('0') << counter-- << std::flush;
			sleep(1);
		}
		std::cout << "\nRetry again!!!\n" << std::flush;
		counter = 10;
		close(bot_socket_);
		std::cout << "In readbuffer file name  = " << info_file_ << std::endl;
		init_bot();
	}
	if (Bot::signal_)
	{
		close(bot_socket_);
		return;
	}
	std::cout << "before end of readbuffer info_file_ = " << getInfoFile() << std::endl;
	
	std::cout << "buffer = " << buffer << std::endl;
}