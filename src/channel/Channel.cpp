#include "Channel.h"

Channel::Channel(const std::string &name) : name_(name), channel_key_(""), topic_is_set_(false), mode_t_(false), mode_i_(false), mode_k_(false), mode_l_(false), mode_n_(false), limit_(DEFAULT_MAX_CLIENTS)
{

}

Channel::~Channel()
{
}

std::string Channel::getName() const
{
	if (name_.empty())
		throw std::runtime_error("Channel name is empty.");
	return name_;
}

std::map<std::shared_ptr<Client>, bool> Channel::getUsers() const
{
	return users_;
}

std::string Channel::getChannelKey() const
{
	return channel_key_;
}

std::pair<std::string, std::string> Channel::getTopic() const
{
	return topic_;
}

unsigned int Channel::getChannelLimit() const
{
	return limit_;
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

// Get mode_n
bool Channel::getModeN() const
{
	return mode_n_;
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
	topic_is_set_ = true;
	topic_timestamp_ = std::chrono::system_clock::now();
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

void Channel::setModeL(bool mode_l, unsigned int limit)
{
	mode_l_ = mode_l;
	if (mode_l_)
		limit_ = limit;
	else
		limit_ = DEFAULT_MAX_CLIENTS;
}

void Channel::setModeN(bool mode_n)
{
	mode_n_ = mode_n;
}

// Check if the channel is full
bool Channel::isFull() const
{
	unsigned int usercount_ = users_.size();
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

void Channel::addUser(std::shared_ptr<Client> client_ptr, bool is_channel_op)
{
	if (client_ptr)
		users_[client_ptr] = is_channel_op;				   // Add the user with operator status if specified
	return;								   // Return true if user added successfully
}

// Remove a user from the channel
void Channel::removeUser(std::shared_ptr<Client> client_ptr)
{
	if (client_ptr)
		users_.erase(client_ptr);				   // Remove the user from the channel
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
	std::string nick_lowercase = nickname;
	std::transform(nick_lowercase.begin(), nick_lowercase.end(), nick_lowercase.begin(), ::tolower); // Convert the nickname to lowercase
	for (auto const &user : users_)
	{
		std::string user_nick = user.first->getNickname();
		std::transform(user_nick.begin(), user_nick.end(), user_nick.begin(), ::tolower); // Convert the user's nickname to lowercase
		if (user_nick == nick_lowercase)
			return true;
	}
	return false;
}

bool Channel::isOperator(std::shared_ptr<Client> client_ptr)
{
	if (client_ptr)
	{
		auto user = users_.find(client_ptr);
		if (user != users_.end())
			return user->second;
	}
	return false;
}

void Channel::broadcastMessage(const std::shared_ptr<Client> &sender_ptr, const std::string &message, Server* server_ptr)
{
	if (sender_ptr)
	{
		for (const auto &recipient_pair : users_)
		{
			std::shared_ptr<Client> recipient_ptr = recipient_pair.first;
			if (recipient_ptr != sender_ptr) // Don't send the message to the sender
			{
				server_ptr->sendResponse(recipient_ptr->getFd(), message);
			}
		}
	}
}

void Channel::broadcastMessageToAll(const std::string &message, Server* server_ptr)
{
	for (const auto &recipient_pair : users_)
	{
		std::shared_ptr<Client> recipient_ptr = recipient_pair.first;
			server_ptr->sendResponse(recipient_ptr->getFd(), message);
	}
}

bool Channel::canChangeTopic(std::shared_ptr<Client> client_ptr)
{
	if (client_ptr)
	{
		if (isOperator(client_ptr))
			return true;
		else if (client_ptr->getNickname() == topic_.first) // Check if the client is the one who set the topic
			return true;
	}
	return false;
}

bool Channel::isCorrectPassword(const std::string& given_password)
{
	return channel_key_ == given_password;
}

bool Channel::changeOpStatus(std::shared_ptr<Client> client_ptr, bool status)
{
	if (client_ptr)
	{
		auto user = users_.find(client_ptr);
		if (user != users_.end())
		{
			if (user->second != status)
			{
				user->second = status;
				return true;
			}
		}
	}
	return false;
}

bool Channel::isUserInvited(const std::string &nickname) const
{
    return invited_users_.find(nickname) != invited_users_.end();
}

void Channel::addUserToInvitedList(const std::string &nickname)
{
	invited_users_.insert(nickname);
}

void Channel::removeUserFromInvitedList(const std::string &nickname)
{
	invited_users_.erase(nickname);
}

void Channel::sendTopicToClient(const std::shared_ptr<Client> &client_ptr, Server* server_ptr)
{
	if (this->topic_is_set_ == false)
		server_ptr->sendResponse(client_ptr->getFd(), RPL_NOTOPIC(server_ptr->getServerHostname(), client_ptr->getNickname(), getName()));
	else
	{
		std::time_t unix_timestamp = std::chrono::system_clock::to_time_t(topic_timestamp_);
		std::string timestamp_string = std::to_string(unix_timestamp);
		server_ptr->sendResponse(client_ptr->getFd(), RPL_TOPIC(server_ptr->getServerHostname(), client_ptr->getNickname(), getName(), getTopic().second));
		server_ptr->sendResponse(client_ptr->getFd(), RPL_TOPICWHOTIME(server_ptr->getServerHostname(), client_ptr->getNickname(), getName(), getTopic().first, timestamp_string));
	}
}

void Channel::clearTopic(const std::string &nickname)
{
	topic_is_set_ = false;
	topic_.first = nickname;
	topic_.second = "";
	topic_timestamp_ = std::chrono::system_clock::now();
}

bool Channel::hasTopic()
{
	return topic_is_set_;
}

std::chrono::time_point<std::chrono::system_clock> const	&Channel::getChannelCreationTimestamps() const
{
	return channel_creation_timestamps_;
}

void	Channel::setChannelCreationTimestamps()
{
	channel_creation_timestamps_ = std::chrono::system_clock::now();
}

bool Channel::isEmpty()
{
	return (users_.size() == 0);
}