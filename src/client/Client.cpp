#include "Client.h"

Client::Client(const int &fd, const std::string &nickname, const std::string &username, const std::string &ipaddress) : fd_(fd), registered_(true), nickname_(nickname), username_(username), ip_address_(ipaddress)
{
	commandMap["JOIN"] = &Client::handleJoin;
	commandMap["NICK"] = &Client::handleNick;
	commandMap["PRIVMSG"] = &Client::handlePrivmsg;
	commandMap["QUIT"] = &Client::handleQuit;
}

Client::~Client()
{
}

// getters

int Client::getFd()
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

std::string Client::getRealname()
{
	return this->realname_;
}

std::string Client::getIpAddress()
{
	return this->ip_address_;
}

bool Client::getRegisterStatus()
{
	return this->registered_;
}

// setters

void Client::setFd(int const &fd)
{
	this->fd_ = fd;
}

/// 		nickname cannot be longer than 9 characters!
void Client::setNickname(std::string const &nickname)
{
	this->nickname_ = nickname;
}

void Client::setUsername(std::string const &username)
{
	this->username_ = username;
}

void Client::setHostname(std::string const &hostname)
{
	this->hostname_ = hostname;
}

void Client::setRealname(std::string const &realname)
{
	this->realname_ = realname;
}

void Client::setIpAddress(std::string const &ip_address)
{
	this->ip_address_ = ip_address;
}

void Client::registerClient()
{
	if (this->registered_ == false)
		this->registered_ = true;
}

void Client::unregisterClient()
{
	if (this->registered_ == true)
		this->registered_ = false;
}

void Client::processBuffer()
{
	size_t pos;
	while ((pos = this->buffer.find("\r\n")) != std::string::npos)
	{
		std::string line = this->buffer.substr(0, pos);
		this->buffer.erase(0, pos + 2);
		if (!line.empty() && line.back() == '\r')
			line.pop_back(); // Remove the trailing \r
		processCommand(line, this->fd_);
	}
}

void Client::processCommand(const std::string &line, int fd)
{
	std::istringstream iss(line);
	std::string command;
	iss >> command;
	std::string parameters;
	getline(iss, parameters);

	if (parameters.size() > 0 && parameters[0] == ' ')
		parameters.erase(0, 1); // Remove the leading space
	auto it = commandMap.find(command);
	if (it != commandMap.end())
		(this->*it->second)(parameters, fd);
	else
		std::cerr << "Unknown command: " << command << std::endl;
}

void Client::appendToBuffer(const std::string &data)
{
	this->buffer += data;
}

void Client::handleJoin(const std::string &parameters, int fd)
{
	// Implementation for JOIN command
}

void Client::handleNick(const std::string &parameters, int fd)
{
	
}

void Client::handlePrivmsg(const std::string &parameters, int fd)
{
	// Implementation for PRIVMSG command
}

void Client::handleQuit(const std::string &parameters, int fd)
{
	// Implementation for QUIT command
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