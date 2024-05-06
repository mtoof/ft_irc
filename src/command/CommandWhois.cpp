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
		server_->send_response(fd, ERR_NOSUCHNICK(nickname));
}
