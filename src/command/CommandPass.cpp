#include "Command.h"

void Command::handlePass(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	if (server_->getPassword().empty())
		return;
	if (client_ptr->getRegisterStatus() == true)
	{
		server_->send_response(fd, ERR_ALREADYREGISTERED(client_ptr->getNickname()));
		return;
	}
	std::vector<std::string> parameters = msg.getParameters();
	size_t pos = parameters.front().find_first_not_of(" \t\v");

	if (pos == std::string::npos || parameters.empty())
		server_->send_response(msg.getClientfd(), ERR_NEEDMOREPARAMS(std::string("*"), "PASS"));
	else if (!client_ptr->getRegisterStatus())
	{
		if (parameters.front() == server_->getPassword())
		{
			// client->registerClient(); //This happens when the user has pass username and nick
			client_ptr->setPassword();
		}
		else if (!server_->getPassword().empty() && parameters.front() != server_->getPassword())
		{
			server_->send_response(fd, ERR_INCORPASS(client_ptr->getNickname()));
			server_->send_response(fd, "Connection got rejected by the server\r\n");
			server_->closeDeletePollFd(fd);
			server_->deleteClient(fd);
		}
	}
}
