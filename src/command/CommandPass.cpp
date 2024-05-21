#include "Command.h"

void Command::handlePass(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int client_fd = client_ptr->getFd();
	if (server_ptr_->getPassword().empty())
		return;
	if (client_ptr->getRegisterStatus() == true)
	{
		server_ptr_->sendResponse(client_fd, ERR_ALREADYREGISTERED(server_ptr_->getServerHostname(), client_ptr->getNickname()));
		return;
	}
	std::vector<std::string> parameters = msg.getParameters();
	size_t pos = parameters.front().find_first_not_of(" \t\v");

	if (pos == std::string::npos || parameters.empty())
		server_ptr_->sendResponse(client_fd, ERR_NEEDMOREPARAMS(std::string("*"), "PASS"));
	else if (!client_ptr->getRegisterStatus())
	{
		if (parameters[0] == server_ptr_->getPassword())
			client_ptr->setHasCorrectPassword(true);
		else if (parameters[0] != server_ptr_->getPassword())
		{
			server_ptr_->sendResponse(client_fd, ERR_PASSWDMISMATCH(server_ptr_->getServerHostname(), client_ptr->getNickname()));
			server_ptr_->sendResponse(client_fd, "ERROR: Connection got rejected by the server\r\n");
			server_ptr_->closeDeletePollFd(client_fd);
			server_ptr_->deleteClient(client_fd);
		}
	}
}
