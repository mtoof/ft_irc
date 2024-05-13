#include "Command.h"

void Command::handlePart(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.empty())
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "PART"));
		return;
	}

	std::vector<std::string> channels = split(parameters[0], ',');
	std::string part_message = msg.getTrailer().empty() ? client_ptr->getNickname() : msg.getTrailer(); // default part message is the nickname

	for (const std::string &channel_name : channels)
	{
		std::shared_ptr<Channel> channel_ptr = server_->findChannel(channel_name);
		if (!channel_ptr)
		{
			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
			continue;
		}

		if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
		{
			server_->send_response(fd, ERR_NOTONCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName()));
			continue;
		}
		channel_ptr->removeUser(client_ptr);
		client_ptr->leaveChannel(channel_ptr);
		server_->send_response(fd, ":" + client_ptr->getClientPrefix() + " PART " + channel_name + " :" + part_message + CRLF);
		channel_ptr->broadcastMessage(client_ptr, ":" + client_ptr->getClientPrefix() + " PART " + channel_name + " :" + part_message + CRLF);
	}
}