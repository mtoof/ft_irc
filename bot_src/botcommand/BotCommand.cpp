#include "BotCommand.hpp"

BotCommand::BotCommand(Bot *bot_ptr)
{
    bot_ = bot_ptr;
}

BotCommand::~BotCommand()
{
}

void BotCommand::handleJoin(const BotMessage &msg)
{
    (void)msg;
}

void BotCommand::handleNick(const BotMessage &msg)
{
    (void)msg;
}

void BotCommand::handleUser(const BotMessage &msg)
{
    (void)msg;
}

void BotCommand::handlePrivmsg(const BotMessage &msg)
{
    (void)msg;
}

void BotCommand::handleMode(const BotMessage &msg)
{
    (void)msg;
}

void BotCommand::handleKick(const BotMessage &msg)
{
    (void)msg;
}

void BotCommand::handleInvite(const BotMessage &msg)
{
    (void)msg;
}
