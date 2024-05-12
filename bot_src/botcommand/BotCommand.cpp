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
    std::string command = msg.getCommand();
    int fd = bot_->getServerfd();
    std::cout << "fd = " << fd << std::endl;
    if (command == "451")
    {
        bot_->send_response(fd, RPL_NICK(bot_->getNickname()));
	    bot_->send_response(fd, RPL_USER(bot_->getUsername()));
    }
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
