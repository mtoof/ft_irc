#include "Client.h"

Client::Client (const int &fd, const std::string &nickname, const std::string &username) :
fd_(fd), registered_(true), nickname_(nickname), username_(username)
{
}

Client::~Client()
{

}

// getters

int		Client::getFd()
{
	return this->fd_;
}

std::string Client::getNickname()
{
	return this->nickname_;
}

std::string Client::getUsername()
{
	return this->username_;
}

std::string Client::getHostname()
{
	return this->hostname_;
}

std::string	Client::getRealname()
{
	return this->realname_;
}

std::string Client::getIpAddress()
{
	return this->ip_address_;
}

bool		Client::getRegisterStatus()
{
	return this->registered_;
}

// setters

void		Client::setFd(int const &fd)
{
	this->fd_ = fd;
}

/// 		nickname cannot be longer than 9 characters!
void		Client::setNickname(std::string const &nickname)
{
	this->nickname_ = nickname;
}

void		Client::setUsername(std::string const &username)
{
	this->username_ = username;
}

void		Client::setHostname(std::string const &hostname)
{
	this->hostname_ = hostname;
}

void		Client::setRealname(std::string const &realname)
{
	this->realname_ = realname;
}

void		Client::setIpAddress(std::string const &ip_address)
{
	this->ip_address_ = ip_address;
}

void		Client::registerClient()
{
	if (this->registered_ == false)
		this->registered_ = true;
}

void		Client::unregisterClient()
{
	if (this->registered_ == true)
		this->registered_ = false;
}

// this function is supposed to send a message to client
// void		Client::sendMessage(std::string const &message)
// {
// 	(void) message;
// }

// and this one would receive messages
// std::string	Client::receiveMessage()
// {

// }

// this would make the client join channel,
// maybe this should take a pointer to the channel
// instead of a string
// void		Client::joinChannel(std::string const &channel)
// {

// }