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
	std::string channel_name = parameters[1];

	std::shared_ptr<Client> target_ptr = server_->findClientUsingNickname(nickname);
	if (!target_ptr)
	{
		server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), nickname));
		return;
	}

	std::shared_ptr<Channel> channel_ptr = server_->findChannel(channel_name);
	if (channel_ptr)
	{
		if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
		{
			server_->send_response(fd, ERR_NOTONCHANNEL(channel_name));
			return;
		}
		if (!channel_ptr->isUserInvited(target_ptr->getNickname()) && channel_ptr->isOperator(client_ptr))
			channel_ptr->addUserToInvitedList(target_ptr->getNickname());
		else
		{
			if (channel_ptr->isInviteOnly() && !channel_ptr->isOperator(client_ptr))
			{
				server_->send_response(fd, ERR_CHANOPRIVSNEEDED(channel_name));
				return;
			}
		}
		if (channel_ptr->isUserOnChannel(nickname))
		{
			server_->send_response(fd, ERR_USERONCHANNEL(server_->getServerHostname(), nickname, channel_name));
			return;
		}
	}

	server_->send_response(target_ptr->getFd(), RPL_INVITED(client_ptr->getClientPrefix(), target_ptr->getNickname(), channel_name));
	server_->send_response(fd, RPL_INVITING(server_->getServerHostname(), client_ptr->getNickname(), target_ptr->getNickname(), channel_name));
	if (target_ptr->isAway())
		server_->send_response(fd, RPL_AWAY(nickname, target_ptr->getAwayMessage()));
}
