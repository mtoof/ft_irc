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
	std::cout << "\nRetry again!!!\n"
			  << std::flush;
	close(server_fd_);
	setRegisterStatus(false);
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

void Bot::send_response(int fd, const std::string &response)
{
	std::cout << YELLOW << "Response: "
			  << response << RESET;
	if (send(fd, response.c_str(), response.length(), 0) < 0)
		std::cerr << "Response send() faild" << std::endl;
}

void	Bot::readConfigFile()
{
	std::ifstream config_file(CONFIG_FILE);
	if (config_file.is_open())
	{
		std::stringstream ss;
		ss << config_file.rdbuf();
		std::string line;
		std::cout << RED << "reading the file" << RESET << std::endl;
		while (ss >> line)
		{
			if (line.find(' ') != std::string::npos)
				continue;
			std::cout << RED << "line = " << line << RESET << std::endl;
			forbidden_words_.push_back(line);
		}
	}
}

std::vector<std::string> const &Bot::getForbiddenWords() const
{
	return forbidden_words_;
}

std::vector<std::string> const &Bot::getViolatedUsers() const
{
	return violated_users_;
}

void Bot::insertInViolatedUsers(std::string const &username)
{
	violated_users_.push_back(username);
}