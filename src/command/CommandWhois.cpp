#include "Command.h"

void Command::handleWhois(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();
	std::string nickname = parameters.front();
	std::shared_ptr<Client> whois_client_ptr = server_->findClientUsingNickname(nickname);
	if (whois_client_ptr && client_ptr->getRegisterStatus())
	{
		if (whois_client_ptr->getHostname().empty())
			server_->send_response(fd, RPL_WHOISUSER(server_->getServerHostname(), client_ptr->getNickname(), whois_client_ptr->getNickname(), whois_client_ptr->getUsername(), whois_client_ptr->getIpAddress(), whois_client_ptr->getRealname()));
		else
			server_->send_response(fd, RPL_WHOISUSER(server_->getServerHostname(), client_ptr->getNickname(), whois_client_ptr->getNickname(), whois_client_ptr->getUsername(), whois_client_ptr->getHostname(), whois_client_ptr->getRealname()));
		server_->send_response(fd, RPL_ENDOFWHOIS(server_->getServerHostname(), client_ptr->getNickname(), nickname));
	}
	else
	{
		server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), nickname));
	}
}