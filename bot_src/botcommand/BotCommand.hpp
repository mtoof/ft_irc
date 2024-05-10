#ifndef __BOTCOMMAND_H__
#define __BOTCOMMAND_H__

#include "../headers/colour.h"
#include "../headers/headers.h"
#include "../headers/reply.h"
#include "../bot/Bot.h"
#include "../botmessage/BotMessage.h"

class Bot;
class BotMessage;
class BotCommand
{
private:
	Bot *bot_;

public:

	BotCommand(Bot *bot_ptr);
	~BotCommand();

	void handleJoin(const BotMessage &msg);
	void handleNick(const BotMessage &msg);
	void handleUser(const BotMessage &msg);
	void handlePrivmsg(const BotMessage &msg);
	void handleMode(const BotMessage &msg);
	void handleKick(const BotMessage &msg);
	void handleInvite(const BotMessage &msg);
	// void handleQuit(const BotMessage &msg);
	// void handlePass(const BotMessage &msg);
	// void handleCap(const BotMessage &msg);
	// void handlePing(const BotMessage &msg);
	// void handleWhois(const BotMessage &msg);
	// void handlePart(const BotMessage &msg);
	// void handleTopic(const BotMessage &msg);
	// void handleAway(const BotMessage &msg);
	// void handleWho(const BotMessage &msg);
};

#endif