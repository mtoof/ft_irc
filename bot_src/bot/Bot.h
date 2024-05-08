#ifndef __BOT_H__
#define __BOT_H__

#define DEFAULTPORT 6667
#include "../headers.h"
#include "../colour.h"
#include "../botmessage/BotMessage.h"

class BotMessage;
class Bot
{
private:
	struct addrinfo		addr_info_;
	struct addrinfo		*serv_addr_info_;
	std::string 		server_addr_;
	int 				server_port_;
	std::string 		server_password_;
	std::string			info_file_;
	std::string			buffer_;
	struct pollfd		poll_fd_;
	int					server_fd_;
	static bool 		signal_;

public:
	Bot(std::string &server_address, int &port, std::string &password, std::string const &filename);
	~Bot();
	void 				init_bot();
	std::string const	&getServerAddr() const;
	std::string const	&getServerPassword() const;
	std::string const	&getInfoFile() const;
	int const			&getServerPort() const;
	void				sendInfo();
	static void 		signalhandler(int signum);
	void 				createBotSocket();
	void 				readBuffer();
	void				reConnection();
	void appendToBuffer(const std::string &data);
	void processBuffer();
	void readFile(std::ifstream &info_file);
};

#endif