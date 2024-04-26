/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oandelin <oandelin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 12:14:05 by atoof             #+#    #+#             */
/*   Updated: 2024/04/26 16:50:14 by oandelin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "../headers.h"
#include "../server/Server.h"
#include "../command/Command.h"
#include "../message/Message.h"

class Server;
class Commands;
class Message;

class Client{
	private:
int																fd_;
	bool														registered_;
	bool														password_; //true if server password is set
	std::string													nickname_;
	std::string													username_;
	std::string													hostname_;
	std::string													realname_;
	std::string													ip_address_;
	std::string													client_prefix_;
	std::string													buffer;

//  std::vector<std::shared_ptr<Channel>>	channels_;
//  do we need to monitor ping pong status?

	public:
	Client() = default;
	Client(const int &fd, const std::string &nickname, const std::string &username, const std::string &ipaddress);
	~Client();

	// getters
	int			getFd();
	std::string getNickname();
	std::string getUsername();
	std::string getHostname();
	std::string getRealname();
	std::string getIpAddress();
	bool		getRegisterStatus();
	const std::string &getClientPrefix();

	// setters

	void		setFd(int const &fd);
	void		setNickname(std::string const &nickname);
	void		setUsername(std::string const &username);
	void		setHostname(std::string const &hostname);
	void		setRealname(std::string const &realname);
	void		setIpAddress(std::string const &ip_address);
	void		setPassword();
	void        setClientPrefix();
	
	// member functions
	void		registerClient();
	void		unregisterClient();
	void 		processBuffer(Server *server_ptr);
	void 		appendToBuffer(const std::string& data);
	void		processCommand(Message &message, Server *server_ptr);
	
	// void		sendMessage(std::string const &message);
	// std::string	receiveMessage();
	// void		joinChannel(std::string const &channel); this could maybe take a pointer instead of string?
};

#endif
