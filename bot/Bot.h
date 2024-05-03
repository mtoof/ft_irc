#ifndef __BOT_H__
#define __BOT_H__

#define DEFAULTPORT 6667

#include <iostream>
#include <sstream>
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
#include <map>

class Bot
{
	private:
	std::string server_addr_;
	int 		server_port_;
	std::string server_password_;
	static bool signal_;

	public:
	Bot(std::string &server_address, int &port, std::string &password);
	~Bot();
	void init_bot();
	std::string const &getServerAddr() const;
	std::string const &getServerPassword() const;
	int const &getServerPort() const;
	static void signalhandler(int signum);
};

#endif