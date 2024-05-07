#include "Command.h"

void Command::handleKick(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.size() < 2)
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "KICK"));
		return;
	}
	// Kick format: KICK <channel> <user> [comment] , https://tools.ietf.org/html/rfc2812#section-3.2.8
	std::vector<std::string> channels = split(parameters[0], ',');
	std::vector<std::string> users = split(parameters[1], ',');
	std::string reason = parameters.size() > 2 ? parameters[2] : client_ptr->getNickname();

	if (channels.size() != users.size() && channels.size() != 1)
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "KICK"));
		return;
	}
	for (size_t i = 0; i < users.size(); i++)
	{
		std::string channelName = channels.size() == 1 ? channels[0] : channels[i];
		std::string userName = users[i];
		if (!channel_->isValidChannelName(channelName))
		{
            server_->send_response(fd, ERR_BADCHANMASK(channelName));
            continue;
        }
		std::shared_ptr<Channel> channel = server_->findChannel(channelName);
		if (!channel)
		{
			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channelName));
			continue;
		}
		if (!channel->isUserOnChannel(client_ptr->getNickname()))
		{
			server_->send_response(fd, ERR_NOTONCHANNEL(channelName));
            continue;
        }

		if (!channel->isOperator(client_ptr))
		{
            server_->send_response(fd, ERR_CHANOPRIVSNEEDED(channelName));
            continue;
        }
		std::shared_ptr<Client> target = server_->findClientUsingNickname(userName);
        if (!target || !channel->isUserOnChannel(userName))
		{
            server_->send_response(fd, ERR_USERNOTINCHANNEL(userName, channelName));
            continue;
        }
		channel->removeUser(target);
        server_->send_response(target->getFd(), "KICK " + channelName + " " + client_ptr->getNickname() + " :" + reason);
       // channel->broadcastMessage(client_ptr->getNickname(), client_ptr->getClientPrefix(), userName + " has been kicked from " + channelName + " (" + reason + ")");
	}
}
