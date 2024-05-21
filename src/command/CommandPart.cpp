#include "Command.h"

void Command::handlePart(const Message &msg)
{
	auto tmp_client_ptr = msg.getClientPtr();
	auto lock_client_ptr = tmp_client_ptr.lock();
	int client_fd = lock_client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.empty())
	{
		server_ptr_->sendResponse(client_fd, ERR_NEEDMOREPARAMS(lock_client_ptr->getClientPrefix(), "PART"));
		return;
	}

	std::vector<std::string> channels = split(parameters[0], ',');
	std::string part_message = msg.getTrailer().empty() ? lock_client_ptr->getNickname() : msg.getTrailer(); // default part message is the nickname

	for (const std::string &channel_name : channels)
	{
		std::shared_ptr<Channel> channel_ptr = server_ptr_->findChannel(channel_name);
		if (!channel_ptr)
		{
			server_ptr_->sendResponse(client_fd, ERR_NOSUCHCHANNEL(server_ptr_->getServerHostname(), lock_client_ptr->getNickname(), channel_name));
			continue;
		}

		if (!channel_ptr->isUserOnChannel(lock_client_ptr->getNickname()))
		{
			server_ptr_->sendResponse(client_fd, ERR_NOTONCHANNEL(server_ptr_->getServerHostname(), lock_client_ptr->getNickname(), channel_ptr->getName()));
			continue;
		}
		channel_ptr->removeUser(lock_client_ptr);
		lock_client_ptr->leaveChannel(channel_ptr);
		server_ptr_->sendResponse(client_fd, RPL_PART(lock_client_ptr->getClientPrefix(), channel_name, part_message));
		channel_ptr->broadcastMessage(lock_client_ptr, RPL_PART(lock_client_ptr->getClientPrefix(), channel_name, part_message), server_ptr_);
		if (channel_ptr->isEmpty())
		{
			server_ptr_->deleteChannel(channel_ptr->getName());
			return;
		}
	}
}