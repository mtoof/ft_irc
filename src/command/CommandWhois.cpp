#include "Command.h"

/*
	TODO: getIdleTime()
*/
void Command::handleWhois(const Message &msg) {
    std::shared_ptr<Client> client_ptr = msg.getClientPtr();
    int fd = client_ptr->getFd();
    std::vector<std::string> parameters = msg.getParameters();
    std::string targetServer;

    if (parameters.empty()) {
        server_->send_response(fd, ERR_NONICKNAMEGIVEN(client_ptr->getClientPrefix()));
        return;
    }

    if (parameters.size() > 1 && server_->getServerHostname() == parameters[0])
	{
		std::string normalizedServerName = server_->toLower(parameters[0]);
        std::string localServerName = server_->toLower(server_->getServerHostname());

        if (localServerName == normalizedServerName)
		{
            std::string targetServer = parameters[0];
            parameters.erase(parameters.begin());
        }
		else
		{
            server_->send_response(fd, ERR_NOSUCHSERVER(parameters[0]));
            return;
        }
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

            if (whois_client_ptr->isAway())
                server_->send_response(fd, RPL_AWAY(whois_client_ptr->getNickname(), whois_client_ptr->getAwayMessage()));

            if (whois_client_ptr->isOperator())
                server_->send_response(fd, RPL_WHOISOPERATOR(whois_client_ptr->getNickname()));
            server_->send_response(fd, RPL_WHOISSERVER(whois_client_ptr->getNickname(), server_->getServerHostname(), "Server Info"));
            //TODO: server_->send_response(fd, RPL_WHOISIDLE(whois_client_ptr->getNickname(),  whois_client_ptr->getIdleTime(), "Server Info"));

            server_->send_response(fd, RPL_WHOISCHANNELS(whois_client_ptr->getNickname(), whois_client_ptr->getChannelName()));
        }
        if (!found)
            server_->send_response(fd, ERR_NOSUCHNICK(mask));
    }

    server_->send_response(fd, RPL_ENDOFWHOIS(server_->getServerHostname(), client_ptr->getNickname(), parameters.front()));
}
