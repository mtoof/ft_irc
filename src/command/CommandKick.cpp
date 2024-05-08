#include "Command.h"

// Kick format: KICK <channel> <user> [comment] , https://tools.ietf.org/html/rfc2812#section-3.2.8
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
	std::vector<std::string> channels = split(parameters[0], ',');
	std::vector<std::string> users = split(parameters[1], ',');
	std::string reason = msg.getTrailer();
	if (reason == "")
		reason = "no reason";

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
		std::shared_ptr<Client> target_ptr = server_->findClientUsingNickname(userName);
		if (!target_ptr)
		{
		server_->send_response(client_ptr->getFd(), ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), userName));
			continue;
		}
		if (!channel->isUserOnChannel(userName))
		{
			server_->send_response(fd, ERR_USERNOTINCHANNEL(client_ptr->getClientPrefix(), client_ptr->getNickname(), userName, channelName));
			continue;
		}

		server_->send_response(fd, RPL_KICK(client_ptr->getClientPrefix(), channelName, target_ptr->getNickname(), reason));
		channel->broadcastMessage(client_ptr, RPL_KICK(client_ptr->getClientPrefix(), channelName, target_ptr->getNickname(), reason));
		channel->removeUser(target_ptr);
		target_ptr->leaveChannel(channel);
	}
}