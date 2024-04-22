#include "Commands.h"
#include "../server/Server.h"

Commands::Commands()
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
	std::shared_ptr<Client> client = findClientUsingFd(fd);
	size_t pos = parameters.find_first_not_of(" \t\v");
	if(pos == std::string::npos || parameters.empty())
		send_response(fd, ERR_NOTENOUGHPARAM(client->getNickname()));
	else if (parameters == password_)
	{
		
	}
}