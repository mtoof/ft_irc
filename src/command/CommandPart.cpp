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
	std::string partMessage = parameters.size() > 1 ? parameters[1] : client_ptr->getNickname(); // default part message is the nickname

	for (const std::string &channelName : channels)
	{
		std::shared_ptr<Channel> channel = server_->findChannel(channelName);
		if (!channel)
		{
			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channelName));
			continue;
		}

		if (!channel->isUserOnChannel(client_ptr->getNickname()))
		{
			server_->send_response(fd, ERR_NOTONCHANNEL(client_ptr->getNickname()));
			continue;
		}

		channel->broadcastMessage(client_ptr->getNickname(), "PART " + channelName + " :" + partMessage);
		channel->removeUser(client_ptr);
		server_->send_response(fd, ":" + client_ptr->getClientPrefix() + " PART " + channelName + " :" + client_ptr->getNickname());
		// if (!channel->getUserCount())
		// 	server_->deleteChannel(channelName);
	}
}