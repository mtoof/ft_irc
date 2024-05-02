#include "Command.h"

void Command::handleWhois(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();
	std::string nickname = parameters.front();
	std::shared_ptr<Client> whois_client_ptr = server_->findClientUsingNickname(nickname);
	if (client_ptr && !client_ptr->getRegisterStatus())
	{
		if (client_ptr->getNickname() == get)
		server_->send_response(client_ptr->getFd(), RPL_WHOISUSER(server_->getServerHostname(), whois_client_ptr->getNickname(), whois_client_ptr->getUsername(), whois_client_ptr->getHostname(), whois_client_ptr->getRealname()));
	}
}