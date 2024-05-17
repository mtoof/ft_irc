#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <memory>
#include <cctype>
#include <set>
#include <chrono>

#include "../client/Client.h"
#include "../common/MagicNumbers.h"
#include "../server/Server.h"

class Client;
class Server;
class Channel
{
	private:
		std::string 										name_; // Channel name
		std::string 										channel_key_; // Channel key
		bool												topic_is_set_; // topic is set
		bool 												mode_t_; // Topic lock mode
		bool												mode_i_; // Invite-only mode
		bool												mode_k_; // Key-protected mode
		bool 												mode_l_; // User limit mode
		bool												mode_n_; // no external messages
		std::string 										mode_; // Channel modes
		unsigned int 										limit_;
		std::set<std::string> 								invited_users_;
		std::pair<std::string, std::string> 				topic_; // Channel topic (author, topic)
		std::map<std::shared_ptr<Client>, bool> 			users_; // Users in the channel and their operator status (true if op)
		std::chrono::time_point<std::chrono::system_clock>	start_channel_timestamps_;
		std::chrono::time_point<std::chrono::system_clock>	topic_timestamp_;
	public:
		Channel(const std::string &name);
		~Channel();

		// Accessor methods
		std::string getName() const;
		std::map<std::shared_ptr<Client>, bool> getUsers() const;
		std::string getChannelKey() const;
		std::pair<std::string, std::string> getTopic() const;
		bool getModeT() const;
		bool getModeI() const;
		bool getModeK() const;
		bool getModeL() const;
		bool getModeN() const;
		std::string const &getMode() const;
		std::chrono::time_point<std::chrono::system_clock> const &getStartChannelTimestamps() const;

		// Mutator methods
		void setName(const std::string &name);
		void setUsers(const std::map<std::shared_ptr<Client>, bool> &users);
		void setUserCount(unsigned int usercount);
		void setChannelKey(const std::string &channel_key);
		void setTopic(const std::pair<std::string, std::string> &topic);
		void setModeT(bool mode_t);
		void setModeI(bool mode_i);
		void setModeK(bool mode_k);
		void setModeL(bool mode_l, unsigned int limit = DEFAULT_MAX_CLIENTS);
		void setModeN(bool mode_n);
		void setStartChannelTimestamps();



		// Functional methods
		bool isFull() const;
		bool isInviteOnly() const;
		bool isPasswordProtected() const;
		void addUser(std::shared_ptr<Client> client, bool isOp);
		void removeUser(std::shared_ptr<Client> client);
		bool isUserOnChannel(std::string const &nickname);
		bool userIsOperator(std::string const &nickname);
		bool isValidChannelName(const std::string& channelName) const;
		void broadcastMessage(const std::shared_ptr<Client> &sender_ptr, const std::string &message, Server* server_ptr);
		void broadcastMessageToAll(const std::string &message, Server* server_ptr);
		bool changeOpStatus(std::shared_ptr<Client> client_ptr, bool status);
		bool isOperator(std::shared_ptr<Client> client_ptr);
		bool canChangeTopic(std::shared_ptr<Client> client_ptr);
		bool isCorrectPassword(const std::string& given_password);
		bool isUserInvited(const std::string &nickname) const;
		void addUserToInvitedList(const std::string &nickname);
		void removeUserFromInvitedList(const std::string &nickname);
		void sendTopicToClient(const std::shared_ptr<Client> &client_ptr, Server* server_ptr);
		void clearTopic(const std::string &nickname);
};

#endif// CHANNEL_H
