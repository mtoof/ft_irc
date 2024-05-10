#include "Command.h"

// https://datatracker.ietf.org/doc/html/rfc2812#section-3.2.7


/**
 * Handles the INVITE command, which invites a user to a channel.
 *
 * The INVITE command is used to invite a user to a channel.
 * If the channel is invite-only, the user must be an operator on the channel to invite others.
 * The command sends a response to the target user indicating that they have been invited to the channel.
 *
 * @param msg The message containing the INVITE command.
 */

void Command::handleInvite(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.size() < 2)
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "INVITE"));
		return;
	}

	std::string nickname = parameters[0];
	std::string channelName = parameters[1];

	std::shared_ptr<Client> target = server_->findClientUsingNickname(nickname);
	if (!target)
	{
		server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), nickname));
		return;
	}

	std::shared_ptr<Channel> channel = server_->findChannel(channelName);
	if (channel)
	{
		if (!channel->isUserOnChannel(client_ptr->getNickname()))
		{
			server_->send_response(fd, ERR_NOTONCHANNEL(channelName));
			return;
		}
		if (!channel->isUserInvited(target->getNickname()) && channel->isOperator(client_ptr))
			channel->addUserToInvitedList(target->getNickname());
		else
		{
			if (channel->isInviteOnly() && !channel->isOperator(client_ptr))
			{
				server_->send_response(fd, ERR_CHANOPRIVSNEEDED(channelName));
				return;
			}
		}
		if (channel->isUserOnChannel(nickname))
		{
			server_->send_response(fd, ERR_USERONCHANNEL(server_->getServerHostname(), nickname, channelName));
			return;
		}
	}

	// server_->send_response(target->getFd(), "INVITE " + nickname + " :" + channelName);
	server_->send_response(fd, RPL_INVITING(server_->getServerHostname(), client_ptr->getNickname(), target->getNickname(), channelName));
	if (target->isAway())
		server_->send_response(fd, RPL_AWAY(nickname, target->getAwayMessage()));
}
