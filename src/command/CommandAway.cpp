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
	std::string away_message = msg.getTrailer();

	if (away_message.empty())
	{
		client_ptr->setAway(false);
		server_->send_response(fd, RPL_UNAWAY(server_->getServerHostname(), client_ptr->getNickname()));
	}
	else
	{
		if (away_message.size() > AWAY_MAX_LENGTH) // if away message is too long, it gets truncated
			away_message = away_message.substr(0, AWAY_MAX_LENGTH);
		client_ptr->setAway(true, away_message);
		server_->send_response(fd, RPL_NOWAWAY(server_->getServerHostname(), client_ptr->getNickname()));
	}
}
