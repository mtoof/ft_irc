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
	auto tmp_client_ptr = msg.getClientPtr();
	auto lock_client_ptr = tmp_client_ptr.lock();
	int client_fd = lock_client_ptr->getFd();
	std::string away_message = msg.getTrailer();

	if (away_message.empty())
	{
		lock_client_ptr->setAway(false);
		server_ptr_->sendResponse(client_fd, RPL_UNAWAY(server_ptr_->getServerHostname(), lock_client_ptr->getNickname()));
	}
	else
	{
		if (away_message.size() > AWAY_MAX_LENGTH) // if away message is too long, it gets truncated
			away_message = away_message.substr(0, AWAY_MAX_LENGTH);
		lock_client_ptr->setAway(true, away_message);
		server_ptr_->sendResponse(client_fd, RPL_NOWAWAY(server_ptr_->getServerHostname(), lock_client_ptr->getNickname()));
	}
}
