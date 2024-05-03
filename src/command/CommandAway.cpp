#include "Command.h"

void Command::handleAway(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::string awayMessage = msg.getTrailer(); // This retrieves the optional <text> parameter

	if (awayMessage.empty())
	{
		client_ptr->setAway(false);
		server_->send_response(fd, RPL_UNAWAY(client_ptr->getNickname(), "You are no longer marked as away"));
	}
	else
	{
		client_ptr->setAway(true, awayMessage);
		server_->send_response(fd, RPL_NOWAWAY(client_ptr->getNickname(), awayMessage));
	}
}
