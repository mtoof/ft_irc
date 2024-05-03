#include "Command.h"

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
    if (parameters.size() < 2)
	{
        server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "PRIVMSG"));
        return;
    }

    std::string recipient = parameters[0];         // Private message in IRC is like: <user> <message>
    std::string message_body = msg.getTrailer();


    std::shared_ptr<Client> recipient_ptr = server_->findClientUsingNickname(recipient);
    if (recipient_ptr)
	{
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
