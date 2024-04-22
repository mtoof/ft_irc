#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <iostream>
#include <string>
#include "../server/Server.h"

class Commands : public Server
{
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