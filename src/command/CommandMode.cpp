#include "Command.h"

void Command::handleMode(const Message &msg)
{
	// (void)msg;
	std::vector <std::string> params = msg.getParameters();
	server_->send_response(msg.getClientfd(), RPL_UMODEIS(msg.getClientPtr()->getNickname(), params.at(1)));
}