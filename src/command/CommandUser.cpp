#include "../server/Server.h"
#include "Command.h"
#include "../debug/debug.h"

void Command::handleUser(const Message &msg)
{
	std::cout << "handleUser called" << std::endl;
	std::vector<std::string> params = msg.getParameters();
	int fd = msg.getClientfd();
	std::shared_ptr client_ptr = msg.getClientPtr();
	if (client_ptr->getRegisterStatus() == true)
		server_->send_response(fd, ERR_ALREADYREGISTERED(client_ptr->getNickname()));
	else if (params.size() == 3 && !msg.getTrailer().empty())
	{
		client_ptr->setUsername(params[0]);
		client_ptr->setRealname(msg.getTrailer());
		if (!client_ptr->getNickname().empty())
		{
			client_ptr->registerClient();
			client_ptr->setClientPrefix();
			server_->welcomeAndMOTD(fd, server_->getServerHostname(), client_ptr->getNickname(), client_ptr->getClientPrefix());
		}
	}
	else
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "USER"));
		return;
	}
}