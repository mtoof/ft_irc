#include "Commands.h"

Commands::Commands()
{
}

Commands::~Commands()
{
}

void Commands::handleJoin(const std::string &parameters, int fd)
{
	(void)parameters;
	// Implementation for JOIN command
}

void Commands::handleNick(const std::string &parameters, int fd)
{
	
	(void)parameters;
	// Implementation for NICK command
}

void Commands::handlePrivmsg(const std::string &parameters, int fd)
{
	(void)parameters;
	// Implementation for PRIVMSG command
}

void Commands::handleQuit(const std::string &parameters, int fd)
{
	(void)parameters;
	// Implementation for QUIT command
}

void Commands::handlePass(const std::string &parameters, int fd)
{
	(void)parameters;
	// Implementation for PASS command
}