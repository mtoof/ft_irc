#include "Command.h"

void Command::handleKill(const Message &msg)
{
	std::shared_ptr <Client> client_ptr = msg.getClientPtr();
	std::string sender_nick = client_ptr->getNickname();
	int fd = msg.getClientfd();
	std::vector<std::string> params = msg.getParameters();
	std::string target_nick, comment, command;
	command = msg.getCommand();
	comment = msg.getTrailer();
	if (comment.empty())
	{
		server_ptr_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), command));
		return;
	}
	if (params.size())
		target_nick = params[0];
	std::map <int, std::shared_ptr<Client>> opers = server_ptr_->getOperatorUsers();
	auto it = opers.find(fd);
	if (it != opers.end() && sender_nick == it->second->getNickname())
	{
		std::shared_ptr <Client> target_client = server_ptr_->findClientUsingNickname(target_nick);
		if (target_client)
		{
			std::string reason = RPL_KILLED(server_ptr_->getServerHostname(), sender_nick, comment);
			server_ptr_->send_response(target_client->getFd(), RPL_KILLMSG(client_ptr->getClientPrefix(), target_client->getNickname(), reason));
			Message quit_msg("QUIT :" + reason, server_ptr_, target_client->getFd());
			target_client->processCommand(quit_msg, server_ptr_);
			return;
		}
		else
		{
			server_ptr_->send_response(fd, ERR_NOSUCHNICK(server_ptr_->getServerHostname(), sender_nick, target_nick));
			return;
		}
	}
	else if (it == opers.end())
		server_ptr_->send_response(fd, ERR_NOPRIVILEGES(sender_nick));
}