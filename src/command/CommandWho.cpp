#include "Command.h"

void Command::sendNamelist(std::shared_ptr<Channel> channel_ptr, std::string user_nickname, int fd)
{
	std::map<std::shared_ptr<Client>, bool> channel_users = channel_ptr->getUsers(); // get the user list
	std::string servername = server_->getServerHostname();
	std::string channel_name = channel_ptr->getName();
	std::string userlist = "";
	for (auto it = channel_users.begin(); it != channel_users.end(); it++) // stitching the user list together in this loop
	{
		std::string flag = "";
		if (it->first->isAway() == true)
			flag += "G";
		else
			flag += "H";
		if (it->first->getModeLocalOp() == true)
			flag += "*";
		server_->send_response(fd, RPL_WHOREPLY(servername, user_nickname, channel_name, it->first->getUsername(), it->first->getHostname(), it->first->getNickname(), flag, it->first->getRealname()));
	}
	server_->send_response(fd, RPL_ENDOFWHO(servername, user_nickname, channel_name));
}

void Command::handleWho(const Message &msg)
{
	std::string command = msg.getCommand();
	std::vector <std::string> params = msg.getParameters();
	int fd = msg.getClientfd();
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	std::shared_ptr <Channel> channel_ptr = server_->findChannel(params[0]);
	if (channel_ptr && channel_ptr->isUserOnChannel(client_ptr->getNickname()))
		sendNamelist(channel_ptr, client_ptr->getNickname(), fd);
	else if (channel_ptr && !channel_ptr->isUserOnChannel(client_ptr->getNickname()))
		server_->send_response(fd, ERR_NOTONCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), params[0]));
}