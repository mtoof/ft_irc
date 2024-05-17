/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atoof <atoof@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 12:13:43 by atoof             #+#    #+#             */
/*   Updated: 2024/04/15 12:13:43 by atoof            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.h"

bool Server::signal_ = false;

Server::Server(const int &port, const std::string &password, const std::stringstream &config_file) : host_(""), port_(port), password_(password)
{
	if (port == -1)
		this->port_ = DEFAULTPORT;
	initOperators(config_file);
	supported_commands_.insert(std::pair("JOIN", &Command::handleJoin));
	supported_commands_.insert(std::pair("NICK", &Command::handleNick));
	supported_commands_.insert(std::pair("PRIVMSG", &Command::handlePrivmsg));
	supported_commands_.insert(std::pair("QUIT", &Command::handleQuit));
	supported_commands_.insert(std::pair("PASS", &Command::handlePass));
	supported_commands_.insert(std::pair("CAP", &Command::handleCap));
	supported_commands_.insert(std::pair("USER", &Command::handleUser));
	supported_commands_.insert(std::pair("PING", &Command::handlePing));
	supported_commands_.insert(std::pair("WHOIS", &Command::handleWhois));
	supported_commands_.insert(std::pair("WHOWAS", &Command::handleWhois));
	supported_commands_.insert(std::pair("PART", &Command::handlePart));
	supported_commands_.insert(std::pair("MODE", &Command::handleMode));
	supported_commands_.insert(std::pair("KICK", &Command::handleKick));
	supported_commands_.insert(std::pair("TOPIC", &Command::handleTopic));
	supported_commands_.insert(std::pair("INVITE", &Command::handleInvite));
	supported_commands_.insert(std::pair("AWAY", &Command::handleAway));
	supported_commands_.insert(std::pair("WHO", &Command::handleWho));
	supported_commands_.insert(std::pair("OPER", &Command::handleOper));
	supported_commands_.insert(std::pair("kill", &Command::handleKill));
	supported_commands_.insert(std::pair("KILL", &Command::handleKill));
}

Server::~Server()
{
	close(this->socket_);
}

void Server::createServerSocket()
{
	struct sockaddr_in6 sa6;
	struct pollfd pfds;

    #ifdef DEBUG_MODE
        std::cout << "Program is compiled in debug mode." << std::endl;
    #endif
	memset(&sa6, 0, sizeof(sa6)); // clear the structure
	sa6.sin6_family = AF_INET6; // set the address family to IPv6
	sa6.sin6_addr = in6addr_any; // set the address to any interface
	sa6.sin6_port = htons(this->port_); // set the port number to the one passed in the constructor (htons converts the port number to network byte order)
	if ((this->socket_ = socket(AF_INET6, SOCK_STREAM, 0)) < 0) // create a socket and check for errors
		debug("socket creation failure", THROW_ERR);
	else
		debug("server socket is running", SUCCESS);
	int ipv6OnlyOpt = 0; // Allows both IPv4 and IPv6 connections
	if (setsockopt(this->socket_, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6OnlyOpt, sizeof(ipv6OnlyOpt)) == -1) // set the socket options and check for errors
		debug("Error while setting socket options!", THROW_ERR);
	else
		debug("server socket options is running", SUCCESS);
	int reuseAddrOpt = 1; // Allow reusing local addresses
	if (setsockopt(this->socket_, SOL_SOCKET, SO_REUSEADDR, &reuseAddrOpt, sizeof(reuseAddrOpt)) == -1) // in this line we set the reuse address option on the socket because we want to reuse the address and port number after the server is closed
		debug("failed to set reuse address on socket", THROW_ERR);
	else
		debug("reuse address on socket is running", SUCCESS);
	if (fcntl(socket_, F_SETFL, O_NONBLOCK) < 0) // set the socket to non-blocking mode because we want to use poll() to check for incoming connections and data on the socket and we don't want to block the server
		debug("Error while setting socket to NON-BLOCKING!", THROW_ERR);
	else
		debug("Server socket is running in NONBLOCK mode", SUCCESS);
	if ((bind(this->socket_, (struct sockaddr *)&sa6, sizeof(sa6))) == -1) // bind the socket to the address and port number, Binding is like setting up a mailbox on a house, telling the postal service (or network messages) where to deliver the letters (or network packets).
		debug("Error while binding server socket!", THROW_ERR);
	else
		debug("binding socket is running", SUCCESS);
	if (listen(this->socket_, SOMAXCONN) < 0) // Listening is akin to saying, "I'm ready to receive visitors" after you've established where you live.
		debug("Error while listen!", THROW_ERR);
	else
		debug("Server is listening", SUCCESS);
	pfds = {this->socket_, POLLIN, 0}; // set the file descriptor for the server socket to the pollfd structure and set the events to POLLIN, this is crucial because we want to check for incoming connections on the server socket and we want to read data from the clients
	fds_.push_back(pfds); // add the server socket to the pollfd vector
	setServerHostname();
	std::cout << GREEN "Server is running on port " << this->port_ << RESET << std::endl; // print a message to the console that the server is running
}

void Server::initServer()
{
	createServerSocket();
	std::cout << "Waiting... " << std::endl;
	int event;
	while (!Server::signal_)
	{
		event = poll(&fds_[0], fds_.size(), -1);
		if (event == -1 && !Server::signal_)
			debug("Server poll error", THROW_ERR);
		for (int index = 0; index < (int)fds_.size(); index++)
		{
			if (fds_[index].revents && POLL_IN)
			{
				if (fds_[index].fd == socket_ && Server::signal_ == false)
					registerNewClient();
				else
					handleClientData(fds_[index].fd);
			}
		}
	}
	closeFds();
}

void Server::registerNewClient()
{
	struct sockaddr_in6	usersocketaddress;
	struct pollfd 		userpollfd;
	socklen_t		 	socketlen;
	int 				userfd;

	memset(&usersocketaddress, 0, sizeof(usersocketaddress));
	socketlen = sizeof(sockaddr_in6);
	userfd = accept(socket_, (sockaddr *)&usersocketaddress, &socketlen);
	if (this->clients_.size() > DEFAULT_MAX_CLIENTS)
	{
		std::string msg = "The maximum number of clients has been reached. We cannot accept any more.";
		send_response(userfd, msg + CRLF);
		close(userfd);
		return;
	}
	if (userfd == -1)
	{
		debug("Accept user socket", FAILED);
		return;
	}
	if (fcntl(userfd, F_SETFL, O_NONBLOCK) < 0)
	{
		debug("Set user fd on NONBLOCK mode", FAILED);
		return;
	}
	userpollfd = {userfd, POLL_IN, 0};
	char *ip;
	if ((ip = extractUserIpAddress(usersocketaddress)) == nullptr)
	{
		debug("Unknown address family", FAILED);
		return;
	}
	std::shared_ptr<Client> newclient = std::make_shared<Client>(userfd, "", "", ip);
	delete ip;
	this->clients_.insert(std::make_pair(userfd, newclient));
	fds_.push_back(userpollfd);
	std::cout << GREEN " <Client " << userfd << "> is trying to establish a connection." RESET << std::endl;
}

/**
 * @brief function for creating new channel
 * could maybe be done by returning a pointer as well
 *
 * @param channel_name
 */
std::shared_ptr<Channel> Server::createNewChannel(std::string const &channel_name)
{
	std::shared_ptr<Channel> new_channel = std::make_shared<Channel>(channel_name);
	this->channels_.insert(std::make_pair(channel_name, new_channel));
	return new_channel;
}

void Server::handleClientData(int fd)
{
	ssize_t readbyte = 0;
	std::vector<std::string> commands;
	char buffer[MAX_MSG_LENGTH] = {};
	memset(buffer, 0, sizeof(buffer));
	std::shared_ptr <Client> client = findClientUsingFd(fd);
	if (!client)
	{
        debug("Failed to find client for fd" + std::to_string(fd), FAILED);
        return;
    }
	readbyte = recv(fd , buffer, MAX_MSG_LENGTH - 1, 0);
	if (readbyte < 0 && !Server::signal_)
	{
		debug("recv function", FAILED);
		return;
	}
	else if (!readbyte)
	{
		disconnectAndDeleteClient(client);
		return;
	}
	else
	{
		client->appendToBuffer(std::string(buffer, readbyte));
		client->processBuffer(this);

	}
}

const std::string & Server::getPassword() const
{
	return password_;
}

std::shared_ptr<Channel> Server::findOrCreateChannel(const std::string& name) {
    auto channel = findChannel(name);
    if (!channel)
	{
        channel = std::make_shared<Channel>(name);
        channels_[name] = channel;
    }
    return channel;
}

void Server::disconnectAndDeleteClient(std::shared_ptr<Client> client_ptr)
{
	int fd = client_ptr->getFd();
	std::cout << RED << "<Client " << fd << "> disconnected" << RESET << std::endl;
	std::vector<std::shared_ptr <Channel>> client_channels = client_ptr->getChannels();
	if (client_channels.size())
	{
		for (auto channel: client_channels)
		{
			std::string disconnect_msg = "PRIVMSG " + channel->getName() + " :" + RED + "disconnected";
			Message msg(disconnect_msg, this, fd);
			client_ptr->processCommand(msg, this);
			channel->removeUser(client_ptr);
			std::cout << RESET << std::endl;
		}
	}
	deleteClient(fd);
	closeDeletePollFd(fd);
}