#include "../server/Server.h"
#include "Commands.h"

Commands::Commands() : server_(nullptr)
{
}

Commands::~Commands()
{
}

void Commands::handleJoin(const std::string &parameters, int fd)
{
	(void)parameters;
	(void)fd;
	// Implementation for JOIN command
}

void Commands::handleNick(const std::string &parameters, int fd)
{
	
	(void)parameters;
	(void)fd;
	// Implementation for NICK command
}

void Commands::handlePrivmsg(const std::string &parameters, int fd)
{
	(void)parameters;
	(void)fd;
	// Implementation for PRIVMSG command
}

void Commands::handleQuit(const std::string &parameters, int fd)
{
	(void)parameters;
	(void)fd;
	// Implementation for QUIT command
}

void Commands::handlePass(const std::string &parameters, int fd)
{
	std::shared_ptr<Client> client = server_->findClientUsingFd(fd);
	size_t pos = parameters.find_first_not_of(" \t\v");
	if(pos == std::string::npos || parameters.empty())
		server_->send_response(fd, ERR_NOTENOUGHPARAM(std::string("*")));
	else if (!client->getRegisterStatus())
	{
		if(parameters == server_->getPassword()) 
			client->registerClient();
		else
			server_->send_response(fd, ERR_INCORPASS(client->getNickname()));
	}
	else
		server_->send_response(fd, ERR_ALREADYREGISTERED(client->getNickname()));
}
