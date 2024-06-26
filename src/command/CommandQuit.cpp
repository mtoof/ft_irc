#include "Command.h"

void Command::handleQuit(const Message &msg)
{
	auto client_ptr = msg.getClientPtr();
	int client_fd = client_ptr->getFd();
	std::string reason = msg.getTrailer().empty() ? "Leaving" : msg.getTrailer();
	server_ptr_->sendResponse(client_fd, "ERROR: Bye, see you soon!\r\n");	
	server_ptr_->disconnectAndDeleteClient(client_ptr, reason);
}