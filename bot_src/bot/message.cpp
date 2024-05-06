#include "Bot.h"
#include "../botmessage/BotMessage.h"

void Bot::readBuffer()
{
	ssize_t readbyte = 0;
	std::vector<std::string> commands;
	char buffer[1024] = {};
	memset(buffer, 0, sizeof(buffer));
	readbyte = recv(server_fd_, buffer, 1024 - 1, 0);
	if (readbyte < 0 && !Bot::signal_)
	{
		std::cerr << "recv function failed" << std::endl;
		return;
	}
	else if (!readbyte)
	{
		std::cout << RED << "Bot lost its connection with the server" << RESET << std::endl;
		reConnection();
		init_bot();
	}
	else
	{
		appendToBuffer(std::string(buffer, readbyte));
		processBuffer();
	}
	if (Bot::signal_)
	{
		close(server_fd_);
		return;
	}
}

void Bot::appendToBuffer(const std::string &data)
{
	buffer_ += data;
}

void Bot::processBuffer()
{
	size_t pos;
	while ((pos = this->buffer_.find("\r\n")) != std::string::npos)
	{
		std::string line = this->buffer_.substr(0, pos);
		this->buffer_.erase(0, pos + 2);
		if (!line.empty() && line.back() == '\r')
			line.pop_back(); // Remove the trailing \r
		//processCommand(line, this->fd_);

		BotMessage message(line); // Parse the message
		if(message.isValidMessage() == true)
		{
			//TODO: send the message to command
			// processCommand(message);
		}
	}
}
