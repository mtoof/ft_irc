/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atoof <atoof@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 12:14:05 by atoof             #+#    #+#             */
/*   Updated: 2024/04/15 12:14:06 by atoof            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>
#include <vector>
#include <memory>

class Client{
	private:
	int										fd_;
	bool									registered_;
	std::string								nickname_; // can't be longer than 9 characters
	std::string								username_;
	std::string								hostname_;
	std::string								realname_;
	std::string								ip_address_;
//  std::vector<std::shared_ptr<Channel>>	channels_;
//  do we need to monitor ping pong status?

	public:
	Client() = default;
	Client(const int &fd, const std::string &nickname, const std::string &username);
	~Client();

	// getters
	int			getFd();
	std::string getNickname();
	std::string getUsername();
	std::string getHostname();
	std::string getRealname();
	std::string getIpAddress();
	bool		getRegisterStatus();

	// setters

	void		setFd(int const &fd);
	void		setNickname(std::string const &nickname);
	void		setUsername(std::string const &username);
	void		setHostname(std::string const &hostname);
	void		setRealname(std::string const &realname);
	void		setIpAddress(std::string const &ip_address);

	// member functions
	void		registerClient();
	void		unregisterClient();
	// void		sendMessage(std::string const &message);
	// std::string	receiveMessage();
	// void		joinChannel(std::string const &channel); this could maybe take a pointer instead of string?
};

#endif