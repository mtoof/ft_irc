#include <string>
#include <vector>
#include <sstream>
#include "../client/Client.h"

class Message{
	private:
	std::string					raw_message_;
	std::string					prefix_;
	std::string					command_;
	std::vector<std::string>	params_;


};