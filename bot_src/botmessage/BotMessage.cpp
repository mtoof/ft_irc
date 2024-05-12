#include "BotMessage.h"

/// @brief constructor for receiving a message from client and getting ready to parse it
/// @param raw_message 
/// @param server 
/// @param clientfd 
BotMessage::BotMessage(std::string raw_message)
: raw_message_(raw_message), valid_message_(false)
{
	std::cout << "Message constructor. Raw message: " << raw_message_ << std::endl;	
	valid_message_ = analyzeMessage();
	// printMessageContents();
}

BotMessage::~BotMessage()
{
	std::cout << "BotMessage destructor" << std::endl;
}

bool BotMessage::analyzeMessage()
{
	std::istringstream iss(raw_message_);
	std::string prefix;
	
	// Extract prefix if present
	if (raw_message_.front() == ':')
	{
		std::getline(iss, prefix, ' '); // Extract prefix up to the first space
		prefix_ = prefix;
	}

	std::string command, param;
		
	iss >> command;
	command_ = command;
	while (iss >> param)
	{
		if (param.front() == ':')
		{
			if (param.size() > 1)
			{
				trailer_ = param;
				while (iss >> param)
					trailer_ += " " + param;
				trailer_.erase(trailer_.begin());
			}
			break;
		}
		parameters_.push_back(param);
	}

	return true;
}

void	BotMessage::printMessageContents()
{
	std::cout << "Printing contents:\n";
	std::cout << "Prefix: " << prefix_ << "\n";
	std::cout << "command: " << command_ << "\n";
	std::cout << "Params:\n";
	for (auto param : parameters_)
	{
		std::cout << param << "\n";
	}
	std::cout << "Trailer trash: " << trailer_ << std::endl;
	
}

std::string BotMessage::getCommand() const
{
	return command_;
}

std::vector<std::string> BotMessage::getParameters() const
{
	return parameters_;
}

bool BotMessage::isValidMessage()
{
	return valid_message_;
}

std::string BotMessage::getTrailer() const
{
	return trailer_;
}
