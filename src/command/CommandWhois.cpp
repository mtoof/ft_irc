#include "Command.h"

void Command::handleWhois(const Message &msg) {
    std::shared_ptr<Client> client_ptr = msg.getClientPtr();
    int fd = client_ptr->getFd();
    std::vector<std::string> parameters = msg.getParameters();
    
    if (parameters.empty()) {
        server_ptr_->send_response(fd, ERR_NONICKNAMEGIVEN(client_ptr->getClientPrefix()));
        return;
    }

    std::vector<std::string> masks = split(parameters.front(), ',');

    for (const auto& mask : masks)
	{
        bool found = false;
        std::vector<std::shared_ptr<Client>> matchedClients = server_ptr_->findClientsByMask(mask);
        for (const auto& whois_client_ptr : matchedClients)
		{
            found = true;
            server_ptr_->send_response(fd, RPL_WHOISUSER(server_ptr_->getServerHostname(), client_ptr->getNickname(),
                                                    whois_client_ptr->getNickname(), whois_client_ptr->getUsername(),
                                                    whois_client_ptr->getHostname(), whois_client_ptr->getRealname()));
        }
        if (!found)
            server_ptr_->send_response(fd, ERR_NOSUCHNICK(server_ptr_->getServerHostname(), client_ptr->getNickname(), mask));
    }

    server_ptr_->send_response(fd, RPL_ENDOFWHOIS(server_ptr_->getServerHostname(), client_ptr->getNickname(), parameters.front()));
}
