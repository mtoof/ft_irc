#include "Command.h"
/**
 * Handles the "away" command for a client.
 *
 * If the client provides an away message,
 * it sets the client's away status to true and sends a response
 * indicating the client is now away with the provided message.
 * If the client does not provide an away message,
 * it sets the client's away status to false and sends a response indicating
 * the client is no longer marked as away.
 *
 * @param msg The message containing the "away" command and any provided away message.
 */

void Command::handleAway(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::string awayMessage = msg.getTrailer();

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
