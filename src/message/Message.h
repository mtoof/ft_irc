#include <string>
#include <vector>
#include <sstream>
#include "../client/Client.h"

class Message{
	private:
	std::string					raw_message_;
	std::string					prefix_;
	std::string					command_;
	std::vector<std::string>	parameters_;
	std::string					trailer_;
	int							client_fd_;
	std::shared_ptr<Server>		server_;
	bool						valid_message_;

	public:
	Message(std::string raw_message, std::shared_ptr<Server> server, int clientfd);
	~Message();

	bool isValidMessage();
	
	bool analyzeMessage(std::string raw_message);
	void setPrefix(std::string raw_message);
	void setCommand(std::string raw_message);
	void setParams(std::string raw_message);
	void setTrailer(std::string raw_message);
	


};