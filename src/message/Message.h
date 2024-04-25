#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "../headers.h"
#include "../client/Client.h"
#include "../server/Server.h"

class Server;
class Client;

class Message{
	private:
	const std::string				raw_message_;
	std::string						prefix_;
	std::string						command_;
	std::vector<std::string>		parameters_;
	std::string						trailer_;
	Server*							server_ptr_;
	const int						client_fd_;
	std::shared_ptr<Client>			client_ptr_;
	bool							valid_message_;

	public:
	Message(std::string raw_message, Server *server, int clientfd);
	~Message();

	bool isValidMessage();
	
	bool analyzeMessage();

	std::string extractPrefix(std::string line);
	bool isValidPrefix(std::string prefix, std::string nickname); // TODO: split analyzemessage() function
	void setPrefix(std::string &prefix);
	void setCommand(std::string raw_message);
	void setParams(std::string raw_message);
	void setTrailer(std::string raw_message);
	
	std::string getCommand();
	std::vector<std::string> getParameters();
	std::string getTrailer();
	void printMessageContents();


};

#endif