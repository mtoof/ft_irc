#include "Message.h"

/// @brief constructor for receiving a message from client and getting ready to parse it
/// @param raw_message 
/// @param server 
/// @param clientfd 
Message::Message(std::string raw_message, std::shared_ptr<Server> server, int clientfd)
: raw_message_(raw_message), server_(server), client_fd_(clientfd), valid_message_(false)
{
	// TODO: Parsing the message and saving it to the members of the class
	// Parser is below, modularize it to subfunctions that save each element of the message to correct members
	// once parsing/analyzing is done, it's sent for COMMAND class (also TODO)
	// THIS IS ABOUT FORMATTING
}

// Message::Message(const std::string &line, Client &client)
// {


// Message::M
// {
// 	std::istringstream iss(line);
// 	std::string prefix;

// 	// Extract prefix if present
// 	if (line.front() == ':')
// 	{
// 		std::getline(iss, prefix, ' '); // Extract prefix up to the first space
// 		std::string temp_prefix = ":" + client->getNickname();
// 		if (prefix != temp_prefix)
// 		{
// 			std::cerr << "Invalid prefix: " << prefix << std::endl;
// 			return;
// 		}
// 	}

// 	std::string command, param;
// 	std::vector<std::string> params;
	
// 	iss >> command;
// 	while (iss >> param)
// 	{
// 		if (param.front() == ':')
// 		{
// 			std::string msg = param.substr(1) + iss.rdbuf()->str();
// 			// 			// 
// 			break;
// 		}
// 		params.push_back(param);
// 	}
// }