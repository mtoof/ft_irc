#include "../server/Server.h"
#include "Command.h"

Command::Command() : server_(nullptr)
{
}

Command::~Command()
{
}

void Command::handleJoin(const std::string &parameters, int fd)
{
	(void)parameters;
	(void)fd;
	// Implementation for JOIN command
}

void Command::handleNick(const std::string &parameters, int fd)
{
	
	(void)parameters;
	(void)fd;
	// Implementation for NICK command
}

void Command::handlePrivmsg(const std::string &parameters, int fd)
{
	(void)parameters;
	(void)fd;
	// Implementation for PRIVMSG command
}

void Command::handleQuit(const std::string &parameters, int fd)
{
	(void)parameters;
	(void)fd;
	// Implementation for QUIT command
}

void Command::handlePass(const std::string &parameters, int fd)
{
	std::shared_ptr<Client> client = server_->findClientUsingFd(fd);
	size_t pos = parameters.find_first_not_of(" \t\v");
	if(pos == std::string::npos || parameters.empty())
		server_->send_response(fd, ERR_NOTENOUGHPARAM(std::string("*")));
	else if (!client->getRegisterStatus())
	{
		if(&parameters == server_->getPassword())
			client->registerClient();
		else
			server_->send_response(fd, ERR_INCORPASS(std::string("*")));
	}
	else
		server_->send_response(fd, ERR_ALREADYREGISTERED(client->getNickname()));
}
