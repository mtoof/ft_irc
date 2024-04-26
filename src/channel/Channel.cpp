#include "Channel.h"
#include "Client.h" // Include the Client class header

// Constructor
Channel::Channel(const std::string &name)
	: name_(name), usercount_(0), channel_key_(""), mode_t_(false), mode_i_(false), mode_k_(false), mode_l_(false)
{
}

// Destructor
Channel::~Channel()
{
}

// Get channel name
std::string Channel::getName() const
{
	return name_;
}

// Get users
std::map<std::shared_ptr<Client>, bool> Channel::getUsers() const
{
	return users_;
}

// Get user count
unsigned int Channel::getUserCount() const
{
	return usercount_;
}

// Get channel key
std::string Channel::getChannelKey() const
{
	return channel_key_;
}

// Get topic
std::pair<std::string, std::string> Channel::getTopic() const
{
	return topic_;
}

// Get mode_t
bool Channel::getModeT() const
{
	return mode_t_;
}

// Get mode_i
bool Channel::getModeI() const
{
	return mode_i_;
}

// Get mode_k
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
}

// Check if the channel is full
bool Channel::isFull() const
{
	return usercount_ >= MAX_CLIENTS;
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

void Channel::addUser(std::shared_ptr<Client> client, bool isOp)
{
	std::lock_guard<std::mutex> lock(mtx);
	users_[client] = isOp;
	usercount_ = users_.size();
}

void Channel::removeUser(std::shared_ptr<Client> client)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (users_.erase(client))
		usercount_ = users_.size();
}