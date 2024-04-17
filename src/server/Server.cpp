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

Server::Server(int port, std::string password) : host_("localhost"), port_(port), password_(password)
{
	if (port == -1)
		this->port_ = DEFAULTPORT;
	this->running_ = 1;
}

Server::~Server()
{
	close(this->socket_);
}

void Server::createServerSocket()
{
	struct sockaddr_in6 sa6;
	struct pollfd pfds;

	memset(&sa6, 0, sizeof(sa6)); // clear the structure
	sa6.sin6_family = AF_INET6; // set the address family to IPv6
	sa6.sin6_addr = in6addr_any; // set the address to any interface
	sa6.sin6_port = htons(this->port_); // set the port number to the one passed in the constructor (htons converts the port number to network byte order)
	if ((this->socket_ = socket(AF_INET6, SOCK_STREAM, 0)) < 0) // create a socket and check for errors
		throw(std::runtime_error("socket creation failure"));
	int ipv6OnlyOpt = 0; // Allows both IPv4 and IPv6 connections
	if (setsockopt(this->socket_, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6OnlyOpt, sizeof(ipv6OnlyOpt)) == -1) // set the socket options and check for errors
		throw(std::runtime_error("Error while setting socket options!"));
	int reuseAddrOpt = 1; // Allow reusing local addresses
	if (setsockopt(this->socket_, SOL_SOCKET, SO_REUSEADDR, &reuseAddrOpt, sizeof(reuseAddrOpt)) == -1) // in this line we set the reuse address option on the socket because we want to reuse the address and port number after the server is closed
		throw(std::runtime_error("failed to set reuse address on socket"));
	if (fcntl(socket_, F_SETFL, O_NONBLOCK) < 0) // set the socket to non-blocking mode because we want to use poll() to check for incoming connections and data on the socket and we don't want to block the server
    	throw std::runtime_error("Error while setting socket to NON-BLOCKING!");
	if ((bind(this->socket_, (struct sockaddr *)&sa6, sizeof(sa6))) == -1) // bind the socket to the address and port number, Binding is like setting up a mailbox on a house, telling the postal service (or network messages) where to deliver the letters (or network packets).
		throw(std::runtime_error("Error while binding a socket!"));
	if (listen(this->socket_, SOMAXCONN) < 0) // Listening is akin to saying, "I'm ready to receive visitors" after you've established where you live.
		throw(std::runtime_error("Error while listen!"));
	pfds = {this->socket_, POLLIN, 0}; // set the file descriptor for the server socket to the pollfd structure and set the events to POLLIN, this is crucial because we want to check for incoming connections on the server socket and we want to read data from the clients
	fds_.push_back(pfds); // add the server socket to the pollfd vector
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
			throw std::runtime_error("Server poll error");
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
	std::shared_ptr<Client> newclient = std::make_shared<Client>();
	userfd = accept(socket_, (sockaddr *)&usersocketaddress, &socketlen);
	if (userfd == -1)
	{
		std::cout << "User accept" << std::endl;
		return;
	}
	if (fcntl(userfd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cout << "User fcntl" << std::endl;
		return;
	}
	userpollfd = {userfd, POLL_IN, 0};
	newclient->setFd(userfd);
	if (extractUserIpAddress(usersocketaddress, newclient) < 0)
	{
		std::cerr << "Unknown address family" << std::endl;
		return;
	}
	newclient->setFd(userfd);
	this->clients_.insert({userfd, newclient});
	fds_.push_back(userpollfd);
}

void Server::handleClientData(int fd)
{
	ssize_t readbyte = 0;
	char buffer[MAX_MSG_LENGTH] = {};

	readbyte = recv(fd , buffer, MAX_MSG_LENGTH, 0);
	if (readbyte < 0 && !Server::signal_)
	{
		perror("Error recv message:");
		return;
	}
	else if (!readbyte)
	{
		std::cout << RED << "<Client " << fd << "> disconnected" << RESET << std::endl;
		deleteClient(fd);
		closeDeletePollFd(fd);
		return;
	}
}
