#include "../server/Server.h"
#include "Command.h"
#include "../debug/debug.h"

void Command::handleUser(const Message &msg)
{
	std::vector<std::string> params = msg.getParameters();
	int client_fd = msg.getClientfd();
	auto tmp_client_ptr = msg.getClientPtr();
	auto lock_client_ptr = tmp_client_ptr.lock();
	if (!server_ptr_->hasClientSentPass(lock_client_ptr))
		return;
	if (lock_client_ptr->getRegisterStatus() == true)
		server_ptr_->sendResponse(client_fd, ERR_ALREADYREGISTERED(server_ptr_->getServerHostname(), lock_client_ptr->getNickname()));
	else if (params.size() == 3 && !msg.getTrailer().empty())
	{
		lock_client_ptr->setUsername(params[0]);
		lock_client_ptr->setHostname(params[2]);
		lock_client_ptr->setRealname(msg.getTrailer());
		if (!lock_client_ptr->getNickname().empty())
		{
			lock_client_ptr->registerClient();
			lock_client_ptr->setClientPrefix();
			server_ptr_->welcomeAndMOTD(client_fd, server_ptr_->getServerHostname(), lock_client_ptr->getNickname(), lock_client_ptr->getClientPrefix());
		}
	}
	else
	{
		server_ptr_->sendResponse(client_fd, ERR_NEEDMOREPARAMS(lock_client_ptr->getClientPrefix(), "USER"));
		return;
	}
}