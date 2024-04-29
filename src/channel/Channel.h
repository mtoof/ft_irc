#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <mutex>
#include <memory>
#include "../client/Client.h"

class Client;


#define MAX_CLIENTS 50
#define MAX_CHANNELS 10

class Channel
{
	private:
		mutable std::mutex 						mtx;
		std::string 							name_;
		std::map<std::shared_ptr<Client>, bool> users_; // Users in the channel and their operator status (true if op)
		unsigned int 							usercount_; // Number of users in the channel
		std::string 							channel_key_; // Channel key
		std::pair<std::string, std::string> 	topic_; // Channel topic
		bool 									mode_t_; // Topic lock mode
		bool									mode_i_; // Invite-only mode
		bool									mode_k_; // Key-protected mode
		bool 									mode_l_; // User limit mode

	public:
		Channel(const std::string &name);
		~Channel();

		// Accessor methods
		std::string getName() const;
		std::map<std::shared_ptr<Client>, bool> getUsers() const;
		unsigned int getUserCount() const;
		std::string getChannelKey() const;
		std::pair<std::string, std::string> getTopic() const;
		bool getModeT() const;
		bool getModeI() const;
		bool getModeK() const;
		bool getModeL() const;

		// Mutator methods
		void setName(const std::string &name);
		void setUsers(const std::map<std::shared_ptr<Client>, bool> &users);
		void setUserCount(unsigned int usercount);
		void setChannelKey(const std::string &channel_key);
		void setTopic(const std::pair<std::string, std::string> &topic);
		void setModeT(bool mode_t);
		void setModeI(bool mode_i);
		void setModeK(bool mode_k);
		void setModeL(bool mode_l);

		// Functional methods
		bool isFull() const;
		bool isInviteOnly() const;
		bool isPasswordProtected() const;
		void addUser(std::shared_ptr<Client> client, bool isOp);
		void removeUser(std::shared_ptr<Client> client);
};

#endif // CHANNEL_H
