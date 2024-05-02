#include "Command.h"

void Command::handleWhois(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	std::cout << RED "client nick = " << client_ptr->getNickname() << std::endl; 
	std::cout << RED "client OLD nick = " << client_ptr->getOldNickname() << RESET << std::endl; 
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();
	std::string nickname = parameters.front();
	std::shared_ptr<Client> whois_client_ptr = server_->findClientUsingNickname(nickname);
	if (whois_client_ptr && client_ptr->getRegisterStatus())
	{
		std::cout  << RED << "here first condition" << RESET << std::endl;
		server_->send_response(fd, RPL_WHOISUSER(server_->getServerHostname(), whois_client_ptr->getNickname(), whois_client_ptr->getUsername(), whois_client_ptr->getHostname(), whois_client_ptr->getRealname()));
		server_->send_response(fd, RPL_ENDOFWHOIS(server_->getServerHostname(), whois_client_ptr->getNickname()));
	}
	if (!whois_client_ptr && !client_ptr->getOldNickname().compare(nickname))
	{
		std::cout << RED "Server sending the whois reply " RESET << std::endl;
		server_->send_response(fd, RPL_WHOISUSER(server_->getServerHostname(), client_ptr->getNickname(), client_ptr->getUsername(), client_ptr->getHostname(), client_ptr->getRealname()));
		server_->send_response(fd, RPL_ENDOFWHOIS(server_->getServerHostname(), client_ptr->getNickname()));
	}
}