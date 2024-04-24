#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <iostream>
#include <string>
#include "../server/Server.h"

class Server;

class Commands
{
private:
	Server *server_;

public:

	Commands();
	~Commands();

	void handleJoin(const std::string &parameters, int fd);
	void handleNick(const std::string &parameters, int fd);
	void handlePrivmsg(const std::string &parameters, int fd);
	void handleQuit(const std::string &parameters, int fd);
	void handlePass(const std::string &parameters, int fd);
};

#endif