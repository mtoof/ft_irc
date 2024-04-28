#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <iostream>
#include <string>
#include "../server/Server.h"
#include "../message/Message.h"
#include "../channel/Channel.h"
#include "../common/MagicNumbers.h"


class Server;
class Message;
class Command
{
private:
	Server *server_;
		
public:

	Command(Server *server_ptr);
	~Command();

	void handleJoin(const Message &msg);
	void handleNick(const Message &msg);
	void handleUser(const Message &msg);
	void handlePrivmsg(const Message &msg);
	void handleQuit(const Message &msg);
	void handlePass(const Message &msg);
	void handleCap(const Message &msg);
};

#endif