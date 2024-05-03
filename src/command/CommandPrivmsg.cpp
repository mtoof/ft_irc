#include "Command.h"

//https://tools.ietf.org/html/rfc2812#section-3.3.1
void Command::handlePrivmsg(const Message &msg)
{
    std::shared_ptr<Client> client_ptr = msg.getClientPtr();
    int fd = client_ptr->getFd();


    if (!client_ptr->getRegisterStatus())
	{
        server_->send_response(fd, ERR_NOTREGISTERED(server_->getServerHostname()));
        return;
    }

    std::vector<std::string> parameters = msg.getParameters();
    if (parameters.empty())
	{
        server_->send_response(fd, ERR_NORECIPIENT(client_ptr->getClientPrefix(), "PRIVMSG"));
        return;
    }

    if (parameters.size() < 2)
	{
        server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "PRIVMSG"));
        return;
    }

    std::string recipient = parameters[0];         // Private message in IRC is like: <user> <message>
    std::string message_body = msg.getTrailer();

	if (message_body.empty())
	{
		server_->send_response(fd, ERR_NOTEXTTOSEND(client_ptr->getClientPrefix()));
		return;
	}

	// Check for host or server mask targeting, which is reserved for operators
    if ((recipient[0] == '#' || recipient[0] == '$') && !channel_->isOperator(client_ptr)) {
        server_->send_response(fd, ERR_NOPRIVILEGES(client_ptr->getClientPrefix()));
        return;
    }

	    // Handling wildcards in top-level domain
    if (recipient.find('.') != std::string::npos && recipient.find_last_of('.') < recipient.size() - 2) // we need to make sure there is at least 2 characters after the last dot
	{
        server_->send_response(fd, ERR_WILDTOPLEVEL(client_ptr->getClientPrefix(), recipient));
        return;
    }

    std::shared_ptr<Client> recipient_ptr = server_->findClientUsingNickname(recipient);
    if (recipient_ptr)
	{
		if (recipient_ptr->isAway())
            server_->send_response(fd, RPL_AWAY(client_ptr->getNickname(), recipient_ptr->getAwayMessage()));
        server_->send_response(recipient_ptr->getFd(), "PRIVMSG " + client_ptr->getNickname() + " :" + message_body);
        return;
    }

    std::shared_ptr<Channel> channel_ptr = server_->findChannel(recipient);
    if (channel_ptr)
	{
        if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
		{
            server_->send_response(fd, ERR_CANNOTSENDTOCHAN(recipient));
            return;
        }
        channel_ptr->broadcastMessage(client_ptr->getNickname(), message_body);
        return;
    }

    server_->send_response(fd, ERR_NOSUCHNICK(recipient));
}
