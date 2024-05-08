#include "Command.h"
#include "../server/Server.h"

void Command::handleJoin(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	if (!client_ptr->getRegisterStatus())
	{
		server_->send_response(fd, ERR_NOTREGISTERED(server_->getServerHostname()));
		return;
	}

	std::vector<std::string> parameters = msg.getParameters();
	if (parameters.empty())
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "JOIN"));
		return;
	}

	std::string channel_name = parameters.front();

	char prefix = channel_name.front();

	if (!channel_->isValidChannelName(channel_name))
	{
		server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
		return;
	}

	std::shared_ptr<Channel> channel_ptr = server_->findChannel(channel_name);
	if (!channel_ptr)
	{
		// Handling based on channel prefix
		switch (prefix)
		{
		case '#': // Standard channels
		case '&': // Local to server
			channel_ptr = server_->createNewChannel(channel_name);
			channel_ptr->addUser(client_ptr, true); // First user becomes the operator
			break;
		case '!': // Safe channels require special handling
			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
			return;
		case '+': // No modes can be set
			channel_ptr = server_->createNewChannel(channel_name);
			channel_ptr->addUser(client_ptr, false);
			break;
		default:
			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
			return;
		}
	}
	else
	{
		if (channel_ptr->isUserOnChannel(client_ptr->getNickname()))
		{
		 	std::cout << "user " << client_ptr->getNickname() << " tried to join channel " << channel_name << "but they are already there" << std::endl;
	 		return;
		}
		if (channel_ptr->isFull())
		{
			server_->send_response(fd, ERR_CHANNELISFULL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
			return;
		}
		if (channel_ptr->isInviteOnly() && !channel_ptr->isUserInvited(client_ptr->getNickname()))
		{
			server_->send_response(fd, ERR_INVITEONLYCHAN(client_ptr->getHostname(), client_ptr->getNickname(), channel_name));
			return;
		}
		if (channel_ptr->isPasswordProtected())
		{
			std::string given_password = parameters.size() > 1 ? parameters[1] : "";
			if (!channel_ptr->isCorrectPassword(given_password))
			{
				server_->send_response(fd, ERR_BADCHANNELKEY(channel_name));
				return;
			}
		}
		channel_ptr->addUser(client_ptr, false);
	}
	client_ptr->joinChannel(channel_ptr);
	server_->send_response(fd, RPL_JOINMSG(client_ptr->getClientPrefix(), channel_name));
	sendNamReplyAfterJoin(channel_ptr, client_ptr->getNickname(), fd);
	broadcastJoinToChannel(channel_ptr, client_ptr);
}


/**
 * @brief Broadcasts a join message to all users in a channel
 *
 * @param channel
 * @param joiningClient
 */
void Command::broadcastJoinToChannel(std::shared_ptr<Channel> channel, std::shared_ptr<Client> joining_client)
{
	auto users = channel->getUsers();
	for (const auto &user : users)
	{
		if (user.first->getFd() != joining_client->getFd())
		{ // Exclude the joining client
			server_->send_response(user.first->getFd(), RPL_JOINMSG(joining_client->getClientPrefix(), channel->getName()));
		}
	}
}

/**
 * @brief this function is for sending the names reply to user after joining a new channel
 * 		  the message lists current users of the channel and their operator status
 *
 * @param channel_ptr
 * @param nickname
 * @param fd
 */
void Command::sendNamReplyAfterJoin(std::shared_ptr<Channel> channel_ptr, std::string nickname, int fd)
{
	std::map<std::shared_ptr<Client>, bool> channel_users = channel_ptr->getUsers(); // get the user list
	std::string servername = server_->getServerHostname();
	std::string channel_name = channel_ptr->getName();
	std::string userlist = "";
	for (auto it = channel_users.begin(); it != channel_users.end(); it++) // stitching the user list together in this loop
	{
		userlist += " ";
		if (it->second == true)
			userlist += "@";
		userlist += it->first->getNickname();
	}
	server_->send_response(fd, RPL_NAMREPLY(servername, nickname, channel_name, userlist));
	server_->send_response(fd, RPL_ENDOFNAMES(servername, nickname, channel_name));
}