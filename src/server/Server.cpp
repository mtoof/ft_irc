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

Server::Server(int port, std::string password) : host_("localhost"), port_(port), password_(password)
{
	if (port == -1)
		this->port_ = DEFAULTPORT;
	this->runnning_ = 1;
	this->server_socket_create();
}

Server::~Server()
{
	close(this->socket_);
}

void Server::server_socket_create()
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
	if ((::bind(this->socket_, (struct sockaddr *)&sa6, sizeof(sa6))) == -1) // bind the socket to the address and port number, Binding is like setting up a mailbox on a house, telling the postal service (or network messages) where to deliver the letters (or network packets).
		throw(std::runtime_error("Error while binding a socket!"));
	if (listen(this->socket_, SOMAXCONN) < 0) // Listening is akin to saying, "I'm ready to receive visitors" after you've established where you live.
		throw(std::runtime_error("Error while listen!"));
	pfds = {this->socket_, POLLIN, 0}; // set the file descriptor for the server socket to the pollfd structure and set the events to POLLIN, this is crucial because we want to check for incoming connections on the server socket and we want to read data from the clients
	fds.push_back(pfds); // add the server socket to the pollfd vector
	std::cout << "Server is running on port " << this->port_ << std::endl; // print a message to the console that the server is running
}