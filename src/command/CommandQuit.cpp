#include "Command.h"

void Command::handleQuit(const Message &msg)
{
	auto tmp_client_ptr = msg.getClientPtr();
	auto lock_client_ptr = tmp_client_ptr.lock();
	int client_fd = lock_client_ptr->getFd();
	std::string reason = msg.getTrailer().empty() ? "Leaving" : msg.getTrailer(); // default part message is the nickname
	server_ptr_->sendResponse(client_fd, "ERROR: Bye, see you soon!\r\n");	
	server_ptr_->disconnectAndDeleteClient(lock_client_ptr, reason);
}