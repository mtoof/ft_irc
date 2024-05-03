#include "Command.h"

void Command::handleMode(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector <std::string> parameters = msg.getParameters();
	std::string target, mode_string, mode_arguments;
	switch(parameters.size())
	{
		case 3:
			
	}
	if (parameters.front() == client_ptr->getNickname())
		server_->send_response(fd, RPL_UMODEIS(client_ptr->getNickname(), parameters.at(1)));
	else if (channelExists(parameters.front()))
	{
		return;
	}
}