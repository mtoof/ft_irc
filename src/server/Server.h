/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atoof <atoof@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 12:13:56 by atoof             #+#    #+#             */
/*   Updated: 2024/04/15 12:13:56 by atoof            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef __SERVER_H__
#define __SERVER_H__

#include "../client/Client.h"
#include "../headers.h"
#include "../debug/debug.h"
#include "../common/reply.h"
#include "../message/Message.h"

#define MAX_MSG_LENGTH 512
#define DEFAULTPORT 6667
#define CRLF "\r\n"

class Client;

class Server
{
private:
	std::string 							host_;
	int 									port_;
	const std::string 						password_;
	int										running_;
	int										socket_;
	std::vector<struct pollfd> 				fds_; // pollfd structure for the server socket
	std::map <int, std::shared_ptr<Client>>	clients_;
	static bool								signal_;
	static void								shutdownServer(const std::string& reason);

public:
	Server(int port, std::string password);
	virtual ~Server();
	static void 			signalHandler(int signum);
	void					createServerSocket();
	void					registerNewClient();
	void					handleClientData(int fd);
	void					initServer();
	void					deleteClient(int fd);
	void					closeDeletePollFd(int fd);
	void					closeFds();
	std::shared_ptr<Client>	findClientUsingFd(int fd) const;
	void					whoGotDisconnected(int fd);
	char*					extractUserIpAddress(struct sockaddr_in6 usersocketaddress);
	void					send_response(int fd, const std::string &response);

//getter

	std::string const * getPassword() const;

};

#endif
