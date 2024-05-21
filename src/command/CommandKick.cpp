#include "Command.h"

// Kick format: KICK <channel> <user> [comment] , https://tools.ietf.org/html/rfc2812#section-3.2.8
void Command::handleKick(const Message &msg)
{
	auto tmp_client_ptr = msg.getClientPtr();
	auto lock_client_ptr = tmp_client_ptr.lock();
	int client_fd = lock_client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.size() < 2)
	{
		server_ptr_->sendResponse(client_fd, ERR_NEEDMOREPARAMS(lock_client_ptr->getClientPrefix(), "KICK"));
		return;
	}
	std::vector<std::string> channels = split(parameters[0], ',');
	std::vector<std::string> targets = split(parameters[1], ',');
	std::string reason = msg.getTrailer();
	if (reason == "")
		reason = "no reason";

	if (channels.size() != targets.size() && channels.size() != 1)
	{
		server_ptr_->sendResponse(client_fd, ERR_NEEDMOREPARAMS(lock_client_ptr->getClientPrefix(), "KICK"));
		return;
	}
	for (size_t i = 0; i < targets.size(); i++)
	{
		std::string channel_name = channels.size() == 1 ? channels[0] : channels[i];
		std::string target_nickname = targets[i];
		if (!isValidChannelName(channel_name))
		{
			server_ptr_->sendResponse(client_fd, ERR_BADCHANMASK(server_ptr_->getServerHostname(), channel_name));
			continue;
		}
		std::shared_ptr<Channel> channel_ptr = server_ptr_->findChannel(channel_name);
		if (!channel_ptr)
		{
			server_ptr_->sendResponse(client_fd, ERR_NOSUCHCHANNEL(server_ptr_->getServerHostname(), lock_client_ptr->getNickname(), channel_name));
			continue;
		}
		if (!channel_ptr->isUserOnChannel(lock_client_ptr->getNickname()))
		{
			server_ptr_->sendResponse(client_fd, ERR_NOTONCHANNEL(server_ptr_->getServerHostname(), lock_client_ptr->getNickname(), channel_name));
			continue;
		}

		if (!channel_ptr->isOperator(lock_client_ptr))
		{
			server_ptr_->sendResponse(client_fd, ERR_CHANOPRIVSNEEDED(server_ptr_->getServerHostname(), channel_name));
			continue;
		}
		std::shared_ptr<Client> target_ptr = server_ptr_->findClientUsingNickname(target_nickname);
		if (!target_ptr)
		{
		server_ptr_->sendResponse(client_fd, ERR_NOSUCHNICK(server_ptr_->getServerHostname(), lock_client_ptr->getNickname(), target_nickname));
			continue;
		}
		if (!channel_ptr->isUserOnChannel(target_nickname))
		{
			server_ptr_->sendResponse(client_fd, ERR_USERNOTINCHANNEL(lock_client_ptr->getClientPrefix(), lock_client_ptr->getNickname(), target_nickname, channel_name));
			continue;
		}

		server_ptr_->sendResponse(client_fd, RPL_KICK(lock_client_ptr->getClientPrefix(), channel_name, target_ptr->getNickname(), reason));
		channel_ptr->broadcastMessage(lock_client_ptr, RPL_KICK(lock_client_ptr->getClientPrefix(), channel_name, target_ptr->getNickname(), reason), server_ptr_);
		channel_ptr->removeUser(target_ptr);
		target_ptr->leaveChannel(channel_ptr);
		if (channel_ptr->isUserInvited(target_ptr->getNickname()))
			channel_ptr->removeUserFromInvitedList(target_ptr->getNickname());
	}
}