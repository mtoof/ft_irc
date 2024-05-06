#include "Command.h"

/*
3.6.2 Whois query

      Command: WHOIS
   Parameters: [ <target> ] <mask> *( "," <mask> )

   This command is used to query information about particular user.
   The server will answer this command with several numeric messages
   indicating different statuses of each user which matches the mask (if
   you are entitled to see them).  If no wildcard is present in the
   <mask>, any information about that nick which you are allowed to see
   is presented.

   If the <target> parameter is specified, it sends the query to a
   specific server.  It is useful if you want to know how long the user
   in question has been idle as only local server (i.e., the server the
   user is directly connected to) knows that information, while
   everything else is globally known.

   Wildcards are allowed in the <target> parameter.

   Numeric Replies:

          * ERR_NOSUCHSERVER             * ERR_NONICKNAMEGIVEN
           RPL_WHOISUSER
          * RPL_WHOISCHANNELS            * RPL_WHOISSERVER
          * RPL_AWAY                     * RPL_WHOISOPERATOR
          * RPL_WHOISIDLE                 ERR_NOSUCHNICK
           RPL_ENDOFWHOIS
   Examples:

   WHOIS wiz                       ; return available user information
                                   about nick WiZ

   WHOIS eff.org trillian          ; ask server eff.org for user

*/
// void Command::handleWhois(const Message &msg)
// {
// 	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
// 	int fd = client_ptr->getFd();
// 	std::vector<std::string> parameters = msg.getParameters();
// 	std::string nickname = parameters.front();
// 	std::shared_ptr<Client> whois_client_ptr = server_->findClientUsingNickname(nickname);
// 	if (whois_client_ptr && client_ptr->getRegisterStatus())
// 	{
// 		if (whois_client_ptr->getHostname().empty())
// 			server_->send_response(fd, RPL_WHOISUSER(server_->getServerHostname(), client_ptr->getNickname(), whois_client_ptr->getNickname(), whois_client_ptr->getUsername(), whois_client_ptr->getIpAddress(), whois_client_ptr->getRealname()));
// 		else
// 			server_->send_response(fd, RPL_WHOISUSER(server_->getServerHostname(), client_ptr->getNickname(), whois_client_ptr->getNickname(), whois_client_ptr->getUsername(), whois_client_ptr->getHostname(), whois_client_ptr->getRealname()));
// 		server_->send_response(fd, RPL_ENDOFWHOIS(server_->getServerHostname(), client_ptr->getNickname(), nickname));
// 	}
// 	else
// 		server_->send_response(fd, ERR_NOSUCHNICK(nickname));
// }

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
