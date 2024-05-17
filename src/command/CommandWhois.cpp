#include "Command.h"

void Command::handleWhois(const Message &msg) {
    std::shared_ptr<Client> client_ptr = msg.getClientPtr();
    int fd = client_ptr->getFd();
    std::vector<std::string> parameters = msg.getParameters();
    
    if (parameters.empty()) {
        server_->send_response(fd, ERR_NONICKNAMEGIVEN(client_ptr->getClientPrefix()));
        return;
    }

    std::vector<std::string> masks = split(parameters.front(), ',');

    for (const auto& mask : masks)
	{
        bool found = false;
        std::vector<std::shared_ptr<Client>> matchedClients = server_->findClientsByMask(mask);
        for (const auto& whois_client_ptr : matchedClients)
		{
            found = true;
            server_->send_response(fd, RPL_WHOISUSER(server_->getServerHostname(), client_ptr->getNickname(),
                                                    whois_client_ptr->getNickname(), whois_client_ptr->getUsername(),
                                                    whois_client_ptr->getHostname(), whois_client_ptr->getRealname()));
        }
        if (!found)
            server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), mask));
    }

    server_->send_response(fd, RPL_ENDOFWHOIS(server_->getServerHostname(), client_ptr->getNickname(), parameters.front()));
}
