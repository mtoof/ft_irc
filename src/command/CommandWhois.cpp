#include "Command.h"

void Command::handleWhois(const Message &msg) {
    auto client_ptr = msg.getClientPtr();
    if (!client_ptr) return;  // Ensure client_ptr is valid

    int fd = client_ptr->getFd();
    const auto& parameters = msg.getParameters();
    if (parameters.empty()) {
        server_->send_response(fd, ERR_NONICKNAMEGIVEN(client_ptr->getClientPrefix()));
        return;
    }

    // Process parameters and ensure all accesses are bounds-checked
    std::string nick = parameters.front();
    auto matchedClients = server_->findClientsByMask(nick);
    if (matchedClients.empty()) {
        server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), nick));
        return;
    }

    for (const auto& whois_client_ptr : matchedClients) {
        if (!whois_client_ptr) continue;  // Ensure the pointer is valid
        // Safe access to channel and client information
        send_responses_based_on_client_info(whois_client_ptr, fd);
    }
}


void Command::send_responses_based_on_client_info(const std::shared_ptr<Client> &whois_client_ptr, int fd)
{
	if (!whois_client_ptr)
		return; // Always verify the pointer's validity

	auto serverName = server_->getServerHostname();
	auto clientPrefix = whois_client_ptr->getClientPrefix();

	// Send basic user information (nick, user, host, real name)
	server_->send_response(fd, RPL_WHOISUSER(
								   serverName,
								   clientPrefix,
								   whois_client_ptr->getNickname(),
								   whois_client_ptr->getUsername(),
								   whois_client_ptr->getHostname(),
								   whois_client_ptr->getRealname()));

	// Check if the client is marked as away
	if (whois_client_ptr->isAway())
		server_->send_response(fd, RPL_AWAY(
									   whois_client_ptr->getNickname(),
									   whois_client_ptr->getAwayMessage()));

	// Send information about the server of the user
	server_->send_response(fd, RPL_WHOISSERVER(
								   whois_client_ptr->getNickname(),
								   serverName,
								   "Server Info" // You might want to replace this with actual server info
								   ));

	// Send the channels the user is on
	std::map<std::string, std::shared_ptr<Channel>> channels = server_->getChannels();
	for (const auto &channel : channels)
	{
        bool found = false;
        std::vector<std::shared_ptr<Client>> matchedClients = server_->findClientsByMask(mask);
        for (const auto& whois_client_ptr : matchedClients)
		{
            found = true;
            server_->send_response(fd, RPL_WHOISUSER(server_->getServerHostname(), client_ptr->getNickname(),
                                                    whois_client_ptr->getNickname(), whois_client_ptr->getUsername(),
                                                    whois_client_ptr->getHostname(), whois_client_ptr->getRealname()));

            // // if (whois_client_ptr->isAway())
            // //     server_->send_response(fd, RPL_AWAY(whois_client_ptr->getNickname(), whois_client_ptr->getAwayMessage()));

            // // if (whois_client_ptr->isOperator())
            // //     server_->send_response(fd, RPL_WHOISOPERATOR(whois_client_ptr->getNickname()));
            // // server_->send_response(fd, RPL_WHOISSERVER(whois_client_ptr->getNickname(), server_->getServerHostname(), "Server Info"));
            // // //TODO: server_->send_response(fd, RPL_WHOISIDLE(whois_client_ptr->getNickname(),  whois_client_ptr->getIdleTime(), "Server Info"));

            // server_->send_response(fd, RPL_WHOISCHANNELS(whois_client_ptr->getNickname(), whois_client_ptr->getChannelName()));
        }
        if (!found)
            server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), mask));
    }

		// Mark the end of WHOIS response
		server_->send_response(fd, RPL_ENDOFWHOIS(
									   serverName,
									   clientPrefix,
									   whois_client_ptr->getNickname()));
	}
}
