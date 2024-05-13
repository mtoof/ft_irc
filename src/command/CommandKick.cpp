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
	std::vector<std::string> targets = split(parameters[1], ',');
	std::string reason = msg.getTrailer();
	if (reason == "")
		reason = "no reason";

	if (channels.size() != targets.size() && channels.size() != 1)
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "KICK"));
		return;
	}
	for (size_t i = 0; i < targets.size(); i++)
	{
		std::string channel_name = channels.size() == 1 ? channels[0] : channels[i];
		std::string target_nickname = targets[i];
		if (!channel_->isValidChannelName(channel_name))
		{
			server_->send_response(fd, ERR_BADCHANMASK(server_->getServerHostname(), channel_name));
			continue;
		}
		std::shared_ptr<Channel> channel_ptr = server_->findChannel(channel_name);
		if (!channel_ptr)
		{
			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
			continue;
		}
		if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
		{
			server_->send_response(fd, ERR_NOTONCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
			continue;
		}

		if (!channel_ptr->isOperator(client_ptr))
		{
			server_->send_response(fd, ERR_CHANOPRIVSNEEDED(server_->getServerHostname(), channel_name));
			continue;
		}
		std::shared_ptr<Client> target_ptr = server_->findClientUsingNickname(target_nickname);
		if (!target_ptr)
		{
		server_->send_response(client_ptr->getFd(), ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), target_nickname));
			continue;
		}
		if (!channel_ptr->isUserOnChannel(target_nickname))
		{
			server_->send_response(fd, ERR_USERNOTINCHANNEL(client_ptr->getClientPrefix(), client_ptr->getNickname(), target_nickname, channel_name));
			continue;
		}

		server_->send_response(fd, RPL_KICK(client_ptr->getClientPrefix(), channel_name, target_ptr->getNickname(), reason));
		channel_ptr->broadcastMessage(client_ptr, RPL_KICK(client_ptr->getClientPrefix(), channel_name, target_ptr->getNickname(), reason));
		channel_ptr->removeUser(target_ptr);
		target_ptr->leaveChannel(channel_ptr);
		if (channel_ptr->isUserInvited(target_ptr->getNickname()))
			channel_ptr->removeUserFromInvitedList(target_ptr->getNickname());
	}
}