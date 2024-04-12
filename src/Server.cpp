/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atoof <atoof@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 20:02:16 by atoof             #+#    #+#             */
/*   Updated: 2024/04/02 20:02:16 by atoof            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string password) : port_(port), password_(password)
{
}

Server::~Server()
{
}

void Server::server_socket_create()
{
	struct sockaddr_in6 sa6;
	struct pollfd pfds;
	
	memset(&sa6, 0, sizeof(sa6));
	sa6.sin6_family = AF_INET6;
	sa6.sin6_addr = in6addr_any;
	sa6.sin6_port = htons(this->port_);
	if (this->socket_ = socket(AF_INET6, SOCK_STREAM, 0) < 0)
		throw(std::runtime_error("socket creation failure"));
	int ipv6OnlyOpt = 0; // Allows both IPv4 and IPv6 connections
	if (setsockopt(this->socket_, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6OnlyOpt, sizeof(ipv6OnlyOpt)) == -1)
		throw(std::runtime_error("Error while setting socket options!"));
	int reuseAddrOpt = 1; // Allow reusing local addresses
	if (setsockopt(this->socket_, SOL_SOCKET, SO_REUSEADDR, &reuseAddrOpt, sizeof(reuseAddrOpt)) == -1)
		throw(std::runtime_error("failed to set reuse address on socket"));
	if (fcntl(socket_, F_SETFL, O_NONBLOCK) < 0)
    	throw std::runtime_error("Error while setting socket to NON-BLOCKING!");
	if (bind(this->socket_, (struct sockaddr *)&sa6, sizeof(sa6)) == -1)
		throw(std::runtime_error("Error while binding a socket!"));
	if (listen(this->socket_, SOMAXCONN) < 0)
		throw(std::runtime_error("Error while listen!"));
	pfds = {this->socket_, POLLIN, 0}; // // add the server socket to the pollfd and set the event to POLLIN for reading data
	fds.push_back(pfds);
}