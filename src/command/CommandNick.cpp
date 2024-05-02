#include "Command.h"

/**
 * @brief handles the NICK command which sets/changes user's nickname
 * 		  TODO: making sure there is no collision between niCkName and nickname
 * 		  this should be handled at Server::findClientUsingNickname by converting
 * 		  all names to lowercase while searching and comparing then
 * @param msg
 */

void Command::handleNick(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	if (!server_->getPassword().empty() && client_ptr->hasSentPassword() == false)
	{
		server_->send_response(fd, "you must send password first"); // this is definitely not the correct reply
		return;
	}
	std::vector<std::string> parameters = msg.getParameters();
	if (parameters.empty())
	{
		server_->send_response(fd, ERR_NONICKNAMEGIVEN(client_ptr->getClientPrefix()));
		return;
	}
	std::string new_nickname = parameters.front(); // desired nickname is the first parameter, we'll just ignore the rest for now
	if (isValidNickname(new_nickname) == false)
	{
		server_->send_response(fd, ERR_ERRONEUSNICK(server_->getServerHostname(), client_ptr->getNickname(), new_nickname));
		return;
	}
	if (isNicknameInUse(new_nickname) == true)
	{
		server_->send_response(fd, ERR_NICKINUSE(server_->getServerHostname(), new_nickname));
		return;
	}
	std::string old_prefix = client_ptr->getClientPrefix(); // this is needed for broadcasting the nickname change
	client_ptr->setNickname(new_nickname);
	client_ptr->setClientPrefix();
	if (!client_ptr->getRegisterStatus() && !client_ptr->getUsername().empty())
	{	
		client_ptr->registerClient();
		server_->welcomeAndMOTD(fd, server_->getServerHostname(), client_ptr->getNickname(), client_ptr->getClientPrefix());
	}
	//else
		server_->send_response(fd, RPL_NICKCHANGE(old_prefix, new_nickname));
	// TODO: broadcast nickname change other users on same channel
	// can be done with this macro: RPL_NICKCHANGECHANNEL(old_prefix, nickname)
	debugWhois(client_ptr);
}

bool Command::isNicknameInUse(std::string const &nickname)
{
	return server_->findClientUsingNickname(nickname) != nullptr;
}

/**
 * @brief	checks whether user's desired nickname fits within RFC2812 standard
 *			allowed chars: a-z, A-Z, 0-9, "[", "]", "\", "_", "-", "^", "|", "{", "}"
 *			however, first character is not allowed to be a digit or "-"
 *
 * @param nickname
 * @return true
 * @return false
 */
bool Command::isValidNickname(std::string &nickname)
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