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

#include <iostream>
#include <string>
#include <vector>	  // std::vector for pollfd structures in the server class
#include <map>		  // std::map for storing clients in the server class
#include <thread>	  // std::thread for running the server in a separate thread
#include <mutex>	  // std::mutex for synchronizing access to the server's clients vector
#include <algorithm>  // std::find_if for checking if a client is already connected
#include <cstdlib>	  // std::exit for exiting the server when all clients disconnect
#include <unistd.h>	  // for sleeping between client checks on UNIX systems (Linux, Mac OS X)
#include <cstdlib>	  // for std::stoi
#include <sys/socket.h> // create an endpoint for communication
#include <sys/types.h>
#include <netinet/in.h>
#include <poll.h> //
#include <fcntl.h>
#include <csignal>
#include <arpa/inet.h>
#include <memory>
#include "../client/Client.h"

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
	std::vector<std::shared_ptr<Client>> 	clients_;
	static void								shutdownServer(const std::string& reason);

public:
	Server(int port, std::string password);
	~Server();

	static bool			signal_;
	static void 		signalHandler(int signum);
	void				createServerSocket();
	void				registerNewClient();
	void				handleClientData(struct pollfd pfd);
	void				initServer();
	void				deleteClient(int fd);
	void				closeDeletePollFd(int fd);
	void				closeFds();
};

#endif
