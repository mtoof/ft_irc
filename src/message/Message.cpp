#include "Message.h"

/// @brief constructor for receiving a message from client and getting ready to parse it
/// @param raw_message 
/// @param server 
/// @param clientfd 
Message::Message(std::string raw_message, Server *server, int clientfd)
: raw_message_(raw_message), server_ptr_(server), client_fd_(clientfd), valid_message_(false)
{
	client_ptr_ = server_ptr_->findClientUsingFd(client_fd_);
	if (!client_ptr_)
	{
		debug("Find client in message constructor", FAILED);
		return;
	}
	std::cout << "Message constructor. Raw message: " << raw_message_ << "\t received from fd: " << client_fd_ << std::endl;
	
	valid_message_ = analyzeMessage();
	printMessageContents();
	// TODO: Parsing the message and saving it to the members of the class
	// Parser is below, modularize it to subfunctions that save each element of the message to correct members
	// once parsing/analyzing is done, it's sent for COMMAND class (also TODO)
	// THIS IS ABOUT FORMATTING
}

Message::~Message()
{
	std::cout << "Message destructor" << std::endl;
}

bool Message::analyzeMessage()
{
	std::istringstream iss(raw_message_);
	std::string prefix;
	
	// Extract prefix if present
	if (raw_message_.front() == ':')
	{
		std::getline(iss, prefix, ' '); // Extract prefix up to the first space
		std::string temp_prefix = ":" + client_ptr_->getNickname();
		if (prefix != temp_prefix)
		{
			std::cerr << "Invalid prefix: " << prefix << std::endl;
			return false;
		}
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
			}
			break;
		}
		parameters_.push_back(param);
	}

	return true;
}

void	Message::printMessageContents()
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

std::string Message::getCommand() const
{
	return command_;
}

std::vector<std::string> Message::getParameters() const
{
	return parameters_;
}

bool Message::isValidMessage()
{
	return valid_message_;
}

std::string Message::getTrailer() const
{
	return trailer_;
}

int Message::getClientfd() const
{
	return client_fd_;
}

std::shared_ptr<Client> Message::getClientPtr() const
{
	return client_ptr_;
}