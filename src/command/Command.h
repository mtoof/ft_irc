#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <iostream>
#include <string>
#include "../server/Server.h"

class Server;

class Command
{
private:
	Server *server_;
		
public:

	Command();
	~Command();

	void handleJoin(const std::string &parameters, int fd);
	void handleNick(const std::string &parameters, int fd);
	void handlePrivmsg(const std::string &parameters, int fd);
	void handleQuit(const std::string &parameters, int fd);
	void handlePass(const std::string &parameters, int fd);
};

#endif