#include "Command.h"

void Command::handleQuit(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int client_fd = client_ptr->getFd();
	std::vector<std::shared_ptr<Channel>> channel_list = client_ptr->getChannels();
	std::string reason = msg.getTrailer().empty() ? "Leaving" : msg.getTrailer(); // default part message is the nickname
	server_ptr_->send_response(client_fd, "ERROR: Bye, see you soon!\r\n");	
	if (!channel_list.empty())
	{
		std::vector<int> fds_sent_to = {};
		for(auto channel_ptr : channel_list)
		{
			for (const auto &recipient_pair : channel_ptr->getUsers())
			{
				std::shared_ptr<Client> recipient_ptr = recipient_pair.first;
				int recipient_fd = recipient_ptr->getFd();
				auto it = std::find(fds_sent_to.begin(), fds_sent_to.end(), recipient_fd);
				if (recipient_ptr != client_ptr && it == fds_sent_to.end()) // Don't send the message to the sender
				{
					server_ptr_->send_response(recipient_fd, RPL_QUIT(client_ptr->getClientPrefix(), reason));
					fds_sent_to.push_back(recipient_fd);
				}
			}
			channel_ptr->removeUser(client_ptr);
			if (channel_ptr->isEmpty())
				server_ptr_->deleteChannel(channel_ptr->getName());
		}
	}
	server_ptr_->disconnectAndDeleteClient(client_ptr);
}

