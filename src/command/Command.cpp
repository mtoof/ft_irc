#include "../server/Server.h"
#include "Command.h"
#include "../debug/debug.h"

Command::Command(Server *server_ptr) : server_(server_ptr)
{
}

Command::~Command()
{
}

// void Command::handleJoin(const Message &msg)
// {
// 	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
// 	int fd = client_ptr->getFd();
// 	std::string client_nick = client_ptr->getNickname();
// 	if (client_ptr->getRegisterStatus() == false)
// 	{
// 		server_->send_response(fd, ERR_NOTREGISTERED(server_->getServerHostname()));
// 		return;
// 	}
//  	std::vector<std::string> parameters = msg.getParameters();
//  	if (parameters.empty()) {
//  		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "JOIN"));
//  		return;
//  	}
//  	std::string channel_name = parameters.front();
// 	if (channel_name.front() != '#') // # missing from beginning?? let's add it!
// 		channel_name.insert(channel_name.begin(), '#');
// 	if (!channelExists(channel_name))
// 	{
// 		server_->createNewChannel(channel_name);
// 		std::shared_ptr<Channel> channel_ptr = server_->findChannel(channel_name);
// 		channel_ptr->addUser(client_ptr, true);
// 		sendNamReplyAfterJoin(channel_ptr, client_nick, fd);
// 	}
// 	else{
// 		std::shared_ptr<Channel> channel_ptr = server_->findChannel(channel_name);
// 		if (channel_ptr->isUserOnChannel(client_nick) == true)
// 		{
// 		std::cout << "user " << client_nick << " tried to join to channel " << channel_name << " but they were already there" << std::endl;
// 	 	return;
// 		}
// 		// TODO: these checks. some of the functions might already exist
// 		//
// 		// this whole section could be separated to another method called canUserJoinChannel
// 		// but we need to implement MODE command first
// 		// if (channel_ptr->isFull() == true)
// 		// {
// 		// 	server_->send_response(fd, ERR_CHANNELISFULL(channel_name));
// 		// 	return;
// 		// }
// 		// if (channel_ptr->isInviteOnly() == true)
// 		// {
// 		// 	if (channel_ptr->isUserInvited(client_ptr->getNickname()) == false)
// 		// 	{
// 		// 		server_->send_response(fd, ERR_INVITEONLYCHAN(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
// 		// 		return;
// 		// 	}
// 		// }
// 		// if (channel_ptr->isPasswordProtected() == true)
// 		// {
// 		// 	if (parameters.size() == 1) {
// 		// 		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "JOIN"));
// 		// 		return;
// 		// 	}
// 		// 	if (parameters.at(2) != channel_ptr->getChannelKey())
// 		// 	{
// 		// 		server_->send_response(fd, ERR_BADCHANNELKEY(channel_name));
// 		// 		return;
// 		// 	}
// 		// }
// 		channel_ptr->addUser(client_ptr, false); // let's add the user on channel
// 		sendNamReplyAfterJoin(channel_ptr, client_nick, fd); // sends NAMES reply to user after joining
// 	}
// 	server_->send_response(fd, RPL_JOINMSG(client_ptr->getClientPrefix(), channel_name)); // sends join message to user
// 	// TODO: send join message to other channel members
// }

/**
 * @brief sends back a PONG when client sends a PING
 * when client sends a PING it contains a token, which has to be sent back to the client
 * this is done in order to track the latency between client and server
 * TODO: keeping track of PING PONG status, time of receiving message to the client class?
 * @param msg
 */
void Command::handlePing(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();
	if (parameters.empty())
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "PING"));
	}
	server_->send_response(fd, PONG(server_->getServerHostname(), parameters.front())); // latter parameter is the token received from client
}

bool Command::channelExists(std::string const &channel_name)
{
	return server_->findChannel(channel_name) != nullptr;
}

void Command::handleCap(const Message &msg)
{
	(void)msg;
	return;
}

std::vector<std::string> Command::split(const std::string &s, char delim)
{
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
		result.push_back(item);
	return result;
}
