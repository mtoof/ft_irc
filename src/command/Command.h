#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <iostream>
#include <string>
#include <regex>
#include "../server/Server.h"
#include "../message/Message.h"
#include "../channel/Channel.h"
#include "../common/MagicNumbers.h"


class Server;
class Channel;
class Message;

class Command
{
private:
	Server *server_;
	Channel *channel_;

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
	void handlePing(const Message &msg);
	void handleWhois(const Message &msg);
	bool isValidNickname(std::string& nickname);
	bool isNicknameInUse(std::string const &nickname);
	bool channelExists(std::string const &channel_name);
	void sendNamReplyAfterJoin(std::shared_ptr<Channel> channel_ptr, std::string nickname, int fd);
	void broadcastJoinToChannel(std::shared_ptr<Channel> channel, std::shared_ptr<Client> joiningClient);
};

#endif
