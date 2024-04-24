#include "Message.h"

Message::Message(const std::string &line, Client &client)
{


Message::M
	std::istringstream iss(line);
	std::string prefix;

	// Extract prefix if present
	if (line.front() == ':')
	{
		std::getline(iss, prefix, ' '); // Extract prefix up to the first space
		std::string temp_prefix = ":" + client->getNickname();
		if (prefix != temp_prefix)
		{
			std::cerr << "Invalid prefix: " << prefix << std::endl;
			return;
		}
	}

	std::string command, param;
	std::vector<std::string> params;
	
	iss >> command;
	while (iss >> param)
	{
		if (param.front() == ':')
		{
			std::string msg = param.substr(1) + iss.rdbuf()->str();
			// 			// 
			break;
		}
		params.push_back(param);
	}
}