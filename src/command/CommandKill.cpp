#include "Command.h"

void Command::handleKill(const Message &msg)
{
	std::shared_ptr <Client> client_ptr = msg.getClientPtr();
	std::string sender_nick = client_ptr->getNickname();
	int client_fd = msg.getClientfd();
	std::vector<std::string> params = msg.getParameters();
	std::string target_nick, comment, command;
	command = msg.getCommand();
	comment = msg.getTrailer();
	if (comment.empty())
	{
		server_ptr_->send_response(client_fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), command));
		return;
	}
	if (params.size())
		target_nick = params[0];
	std::map <int, std::shared_ptr<Client>> server_operators = server_ptr_->getOperatorUsers();
	auto it = server_operators.find(client_fd);
	if (it != server_operators.end() && sender_nick == it->second->getNickname())
	{
		std::shared_ptr <Client> target_client = server_ptr_->findClientUsingNickname(target_nick);
		if (target_client)
		{
			std::string kill_message = RPL_KILLED(server_ptr_->getServerHostname(), sender_nick, comment);
			server_ptr_->send_response(target_client->getFd(), RPL_KILLMSG(client_ptr->getClientPrefix(), target_client->getNickname(), kill_message));
			Message quit_msg("QUIT :" + kill_message, server_ptr_, target_client->getFd());
			target_client->processCommand(quit_msg, server_ptr_);
			return;
		}
		else
		{
			server_ptr_->send_response(client_fd, ERR_NOSUCHNICK(server_ptr_->getServerHostname(), sender_nick, target_nick));
			return;
		}
	}
	else if (it == server_operators.end())
		server_ptr_->send_response(client_fd, ERR_NOPRIVILEGES(sender_nick));
}