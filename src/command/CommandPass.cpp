#include "Command.h"

void Command::handlePass(const Message &msg)
{
	auto tmp_client_ptr = msg.getClientPtr();
	auto lock_client_ptr = tmp_client_ptr.lock();
	int client_fd = lock_client_ptr->getFd();
	if (server_ptr_->getPassword().empty())
		return;
	if (lock_client_ptr->getRegisterStatus() == true)
	{
		server_ptr_->sendResponse(client_fd, ERR_ALREADYREGISTERED(server_ptr_->getServerHostname(), lock_client_ptr->getNickname()));
		return;
	}
	std::vector<std::string> parameters = msg.getParameters();
	size_t pos = parameters.front().find_first_not_of(" \t\v");

	if (pos == std::string::npos || parameters.empty())
		server_ptr_->sendResponse(client_fd, ERR_NEEDMOREPARAMS(std::string("*"), "PASS"));
	else if (!lock_client_ptr->getRegisterStatus())
	{
		if (parameters[0] == server_ptr_->getPassword())
			lock_client_ptr->setHasCorrectPassword(true);
		else if (parameters[0] != server_ptr_->getPassword())
		{
			server_ptr_->sendResponse(client_fd, ERR_INCORPASS(server_ptr_->getServerHostname(), lock_client_ptr->getNickname()));
			server_ptr_->sendResponse(client_fd, RED "Connection got rejected by the server\r\n");
			std::cout << RESET << std::endl;
			server_ptr_->closeDeletePollFd(client_fd);
			server_ptr_->deleteClient(client_fd);
		}
	}
}
