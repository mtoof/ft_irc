#include "Channel.h"

Channel::Channel(const std::string &name) : name_(name), usercount_(0), channel_key_(""), mode_t_(false), mode_i_(false), mode_k_(false), mode_l_(false), limit_(DEFAULT_MAX_CLIENTS)
{
}

Channel::~Channel()
{
}

std::string Channel::getName() const
{
	return name_;
}

std::map<std::shared_ptr<Client>, bool> Channel::getUsers() const
{
	return users_;
}

unsigned int Channel::getUserCount() const
{
	return usercount_;
}

std::string Channel::getChannelKey() const
{
	return channel_key_;
}

std::pair<std::string, std::string> Channel::getTopic() const
{
	return topic_;
}

// Get mode_t (topic lock mode)
bool Channel::getModeT() const
{
	return mode_t_;
}

// Get mode_i (invite-only mode)
bool Channel::getModeI() const
{
	return mode_i_;
}

// Get mode_k (key-protected mode)
bool Channel::getModeK() const
{
	return mode_k_;
}

// Get mode_l
bool Channel::getModeL() const
{
	return mode_l_;
}

// Set channel name
void Channel::setName(const std::string &name)
{
	name_ = name;
}

// Set users
void Channel::setUsers(const std::map<std::shared_ptr<Client>, bool> &users)
{
	users_ = users;
}

// Set user count
void Channel::setUserCount(unsigned int usercount)
{
	usercount_ = usercount;
}

// Set channel key
void Channel::setChannelKey(const std::string &channel_key)
{
	channel_key_ = channel_key;
	if (channel_key_.size() > 0)
		setModeK(true);
	else
		setModeK(false);
}

// Set topic
void Channel::setTopic(const std::pair<std::string, std::string> &topic)
{
	topic_ = topic;
}

// Set mode_t
void Channel::setModeT(bool mode_t)
{
	mode_t_ = mode_t;
}

// Set mode_i
void Channel::setModeI(bool mode_i)
{
	mode_i_ = mode_i;
}

// Set mode_k
void Channel::setModeK(bool mode_k)
{
	mode_k_ = mode_k;
}

// Set mode_l
void Channel::setModeL(bool mode_l)
{
	mode_l_ = mode_l;
	if (mode_l_)
		limit_ = DEFAULT_MAX_CLIENTS;
	else
		limit_ = 0;
}

// Check if the channel is full
bool Channel::isFull() const
{
	return mode_l_ && usercount_ >= limit_;
}

// Check if the channel is invite only
bool Channel::isInviteOnly() const
{
	return mode_i_;
}

// Check if the channel is password protected
bool Channel::isPasswordProtected() const
{
	return mode_k_ && !channel_key_.empty();
}

// Additional method to update topic with validation
void Channel::updateTopic(const std::string &newTopic, const std::string &author, bool isAdmin)
{
	if (mode_t_ && !isAdmin)
		throw std::runtime_error("Topic is locked.");
	topic_ = {author, newTopic}; // Update topic with author and new topic text
}

void Channel::addUser(std::shared_ptr<Client> client, bool isOp)
{
	std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety
	users_[client] = isOp;				   // Add the user with operator status if specified
	usercount_ = users_.size();			   // Update the user count
	return;								   // Return true if user added successfully
}

// Remove a user from the channel
void Channel::removeUser(std::shared_ptr<Client> client)
{
	std::lock_guard<std::mutex> lock(mtx); // We are modifying the users_ map and usercount_ variable in this function and we don't want other threads to access them at the same time
	if (users_.erase(client))
		usercount_ = users_.size();
}

/**
 * @brief helper function for checking if user is on channel
 *
 * @param nickname
 * @return true
 * @return false
 *
 */

bool Channel::isUserOnChannel(std::string const &nickname)
{
	std::string lowerCaseNick = nickname;
	std::transform(lowerCaseNick.begin(), lowerCaseNick.end(), lowerCaseNick.begin(), ::tolower); // Convert the nickname to lowercase
	for (auto const &user : users_)
	{
		std::string userNick = user.first->getNickname();
		std::transform(userNick.begin(), userNick.end(), userNick.begin(), ::tolower); // Convert the user's nickname to lowercase
		if (userNick == lowerCaseNick)
			return true;
	}
	return false;
}

bool Channel::userIsOperator(std::string const &nickname)
{
	//std::string lowerCaseNick = ;
	for (auto const &user : users_)
	{
		if (server_->toLower(user.first->getNickname()) == server_->toLower(nickname) && user.second == true)
			return true;
	}
	return false;
}
bool Channel::isValidChannelName(const std::string& channelName) const
{
	// Regex to match valid channel names
	std::regex pattern("^[&#\\+!][^ ,\\x07]{1,49}$"); // Adjusted for max length of 50 and disallowed characters
	return std::regex_match(channelName, pattern);
}

bool Channel::isOperator(std::shared_ptr<Client> client_ptr)
{
	std::lock_guard<std::mutex> lock(mtx);
	auto user = users_.find(client_ptr);
	if (user != users_.end())
		return user->second;
	return false;
}

void Channel::broadcastMessage(const std::string &sender_nickname, const std::string &sender_prefix, const std::string &message)
{
	std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety while iterating over users

	for (const auto &userPair : users_)
	{
		std::shared_ptr<Client> user = userPair.first;
		if (user->getNickname() != sender_nickname) // Don't send the message to the sender
		{
			std::string fullMessage = ":" + sender_prefix + " PRIVMSG " + this->name_ + " :" + message + CRLF;
			server_->send_response(user->getFd(), fullMessage);
		}
	}
}

bool Channel::canChangeTopic(std::shared_ptr<Client> client_ptr)
{
	if (isOperator(client_ptr))
		return true;
	else if (client_ptr->getNickname() == topic_.first) // Check if the client is the one who set the topic
		return true;
	else
		return false;

}

bool Channel::isCorrectPassword(const std::string& given_password)
{
	return channel_key_ == given_password;
}