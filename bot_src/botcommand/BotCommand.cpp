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
	std::string reply_number = msg.getReplyNumber();
	int fd = bot_->getServerfd();
	if (reply_number == "451")
	{
		bot_->send_response(fd, RPL_NICK(bot_->getNickname()));
		bot_->send_response(fd, RPL_USER(bot_->getUsername()));
	}
}

void BotCommand::handleNick(const BotMessage &msg)
{
	std::string reply_number = msg.getReplyNumber();
	int fd = bot_->getServerfd();
	std::string nick = bot_->getNickname();
	if (reply_number == "433")
	{
		size_t pos = nick.find_last_of("0123456789");
		if(pos != std::string::npos)
		{
			std::string num_in_nick = nick.substr(pos);
			int number;
			std::stringstream(num_in_nick) >> number;
			++number;
			nick.replace(pos, num_in_nick.length(), std::to_string(number));
			bot_->setNickname(nick);
		}
		else
			nick += "_";
		bot_->send_response(fd, RPL_NICK(bot_->getNickname()));
	}
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