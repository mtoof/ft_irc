#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>
#include "../server/Server.h"
#include "../message/Message.h"
#include "../channel/Channel.h"
#include "../common/MagicNumbers.h"
#include "../common/reply.h"


class Server;
class Channel;
class Message;

class Command
{
private:
	Server *server_;
	Channel *channel_;

public:

	Command(Server *server_ptr);
	~Command();

	void handleJoin(const Message &msg);
	void handleNick(const Message &msg);
	void handleUser(const Message &msg);
	void handlePrivmsg(const Message &msg);
	void handleQuit(const Message &msg);
	void handlePass(const Message &msg);
	void handleCap(const Message &msg);
	void handlePing(const Message &msg);
	void handleWhois(const Message &msg);
	void handlePart(const Message &msg);
	void handleMode(const Message &msg);
	void handleKick(const Message &msg);
	void handleTopic(const Message &msg);
	void handleInvite(const Message &msg);
	void handleAway(const Message &msg);
	void handleWho(const Message &msg);
	void handleOper(const Message &msg);
	void handleKill(const Message &msg);
	void handleModeL(std::shared_ptr<Channel> channel, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &lastModeChar);
	void handleModeK(std::shared_ptr<Channel> channel, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &lastModeChar);
	void handleModeO(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, const std::vector<std::string> &modeArguments, bool setMode, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &lastModeChar);
	void handleModeChange(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, char modeChar, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &lastModeChar);
	bool isValidNickname(std::string& nickname);
	bool isNicknameInUse(std::string const &nickname);
	bool channelExists(std::string const &channel_name);
	void sendNamReplyAfterJoin(std::shared_ptr<Channel> channel_ptr, std::string nickname, int fd);
	void sendNamelist(std::shared_ptr<Channel> channel_ptr, std::string nickname, int fd);
	void broadcastJoinToChannel(std::shared_ptr<Channel> channel, std::shared_ptr<Client> joiningClient);
	void leaveAllChannels(std::shared_ptr<Client> client_ptr, const std::string &command);
	std::vector<std::string> split(const std::string &s, char delim);
	void extractMode(const Message &msg, const std::vector<std::string> &params, std::string &mode_string);
	void applyUserMode(std::shared_ptr<Client> client, const std::string& modeString);
	void applyChannelModes(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, const std::string& modeString, const std::vector<std::string>& modeArguments);
	bool applyModeO(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, const std::string& targetNickname, bool setMode);
	void send_responses_based_on_client_info(const std::shared_ptr<Client> &whois_client_ptr, int fd);
	void appendToChangedModeString(bool setMode, std::string& changedModes, char& lastModeChar, char modeChar);
	bool modeRequiresParameter(char mode);
	bool mandatoryModeParameter(char mode);
	void handleChannelMode(std::shared_ptr<Client> client, const std::string& target, const std::string& modeString, const std::vector<std::string>& modeArguments);
	void parseModeString(const std::string &modeString, std::vector<std::pair<char, bool>> &modeChanges, bool &isSettingMode);
	void sendCurrentUserModes(std::shared_ptr<Client> client);
	void appendModeChange(std::string &modeChange, char &lastModeChar, bool isSettingMode, char mode);
	void handleUserMode(std::shared_ptr<Client> client, const std::string &target, const std::string &modeString);

};

#endif
