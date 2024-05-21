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
		bot_->send_response(fd, RPL_PASS(bot_->getServerPassword()));
		bot_->send_response(fd, RPL_NICK(bot_->getNickname()));
		bot_->send_response(fd, RPL_USER(bot_->getUsername()));
		return;
	}
	std::string prefix = msg.getPrefix();
	std::string nickname = prefix.substr(1, prefix.find_first_of("!") - 1);
	std::string channel_name = msg.getParameters().front();
	if (nickname != bot_->getNickname())
	{
		bot_->send_response(fd, RPL_PRIVMSG(channel_name, "Welcome to " + channel_name + ", " + nickname + "!"));
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
		if (pos != std::string::npos)
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

void BotCommand::handlePrivmsg(const BotMessage &msg)
{
	std::string prefix = msg.getPrefix();
	std::string channel_name = msg.getParameters()[0];
	if (prefix[0] == ':')
		prefix.erase(0, 1);
	size_t pos = prefix.find('!');
	std::string offender;
	if (pos != std::string::npos)
		offender = prefix.substr(0, pos);
	std::string line(msg.getTrailer());
	int fd = bot_->getServerfd();
	std::string word;
	std::vector<std::string> forbidden_words = bot_->getForbiddenWords();
	std::vector<std::string> violated_users = bot_->getViolatedUsers();
	if (!forbidden_words.empty())
	{
		for (auto word_it: forbidden_words)
		{
			//std::cout << line.find(word_it) << std::endl;
			if (line.find(word_it) != std::string::npos)
			{
				if (std::find(violated_users.begin(), violated_users.end(), offender) != violated_users.end())
				{
					bot_->send_response(fd, KICK_REQUEST(channel_name, offender + " :You have violated the chat room rules."));
					return;
				}
				else
				{
					bot_->send_response(fd, RPL_PRIVMSG(channel_name, offender + " :This is the last warning, You have violated the chat room rules."));
					bot_->insertInViolatedUsers(offender);
					return;
				}
			}
		}
	}
	if (line.find("lol") != std::string::npos)
	{
		bot_->send_response(fd, RPL_PRIVMSG(channel_name, "   /\\O"));
		bot_->send_response(fd, RPL_PRIVMSG(channel_name, "    /\\/"));  
    	bot_->send_response(fd, RPL_PRIVMSG(channel_name, "   /\\"));    
   		bot_->send_response(fd, RPL_PRIVMSG(channel_name, "  /  \\"));
 		bot_->send_response(fd, RPL_PRIVMSG(channel_name, "LOL  LOL"));
		return;
	}
	if (line.find("haha") != std::string::npos){
		bot_->send_response(fd, RPL_PRIVMSG(channel_name, "THAT WAS HILARIOUS!"));
		return;}
}


void BotCommand::handleKick(const BotMessage &msg)
{
	(void)msg;
}

void BotCommand::handleInvite(const BotMessage &msg)
{
	std::string reply_number = msg.getReplyNumber();
	int fd = bot_->getServerfd();
	std::cout << "reply_number = " << reply_number << std::endl;
	if (reply_number == "INVITE" && msg.getParameters()[0] == bot_->getNickname())
	{
		bot_->send_response(fd, "JOIN " + msg.getTrailer() + CRLF);
	}
}

