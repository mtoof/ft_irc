#include "../server/Server.h"
#include "Command.h"

Command::Command(Server *server_ptr) : server_(server_ptr)
{
}

Command::~Command()
{
}

void Command::handleJoin(const Message &msg)
{
	(void)msg;
	
	// Implementation for JOIN command
	return;
}

void Command::handleNick(const Message &msg)
{
	
	(void)msg;
	
	// Implementation for NICK command
	return;
}

void Command::handlePrivmsg(const Message &msg)
{
	(void)msg;
	
	// Implementation for PRIVMSG command
	return;
}

void Command::handleQuit(const Message &msg)
{
	(void)msg;
	
	// Implementation for QUIT command
	return;
}

void Command::handlePass(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	if (server_->getPassword().empty())
		return;
	if (client_ptr->getRegisterStatus() == false)
	{
		server_->send_response(client_ptr->getFd(), ERR_ALREADYREGISTERED(client_ptr->getNickname()));
		return;
	}
	std::vector<std::string> parameters = msg.getParameters();
	size_t pos = parameters.front().find_first_not_of(" \t\v");
	
	if(pos == std::string::npos || parameters.empty())
		server_->send_response(msg.getClientfd(), ERR_NOTENOUGHPARAM(std::string("*")));
	else if (!client_ptr->getRegisterStatus())
	{
		if(parameters.front() == server_->getPassword()) 
		{
			// client->registerClient(); //This happens when the user has pass username and nick
			client_ptr->setPassword();
		}
		else if (!server_->getPassword().empty() && parameters.front() != server_->getPassword())
			server_->send_response(client_ptr->getFd(), ERR_INCORPASS(client_ptr->getNickname()));
	}
}

void Command::handleCap(const Message &msg)
{
	(void)msg;
	
	// Implementation for CAP command
	return;
}