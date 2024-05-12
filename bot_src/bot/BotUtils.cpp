#include "Bot.h"

/**
 * @brief	checks whether user's desired nickname fits within RFC2812 standard
 *			allowed chars: a-z, A-Z, 0-9, "[", "]", "\", "_", "-", "^", "|", "{", "}"
 *			however, first character is not allowed to be a digit or "-"
 *
 * @param nickname
 * @return true
 * @return false
 */
bool Bot::isValidNickname(std::string nickname)
{
	if (isdigit(nickname.front()) || nickname.front() == '-')
		return false;
	if (nickname.size() > NICK_MAX_LENGTH) // if nickname is too long, it gets truncated
		nickname = nickname.substr(0, NICK_MAX_LENGTH);
	std::regex pattern("([A-Za-z0-9\\[\\]\\\\_\\-\\^|{}])\\w*");
	if (std::regex_match(nickname, pattern))
		return true;
	else
		return false;
}

void Bot::reConnection()
{
	int counter = 10;
	while (counter && !Bot::signal_)
	{
		std::cout << '\r' << std::setw(2) << std::setfill('0') << counter-- << std::flush;
		sleep(1);
	}
	std::cout << "\nRetry again!!!\n" << std::flush;
	close(server_fd_);
	init_bot();
}

std::map<std::string, void (BotCommand::*)(const BotMessage &msg)> const &Bot::getSupportedCommands() const
{
	return supported_commands_;
}

int const &Bot::getServerfd() const
{
	return server_fd_;
}