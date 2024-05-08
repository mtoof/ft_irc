#include "Command.h"

void Command::handleQuit(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::shared_ptr<Channel>> channel_list = client_ptr->getChannels();
	std::string reason = msg.getTrailer().empty() ? "Leaving" : msg.getTrailer(); // default part message is the nickname
	server_->send_response(fd, "ERROR: Bye, see you soon!\r\n");	
	if (!channel_list.empty())
	{
		std::vector<int> fds_sent_to = {};
		for(auto channel : channel_list)
		{
			for (const auto &userPair : channel->getUsers())
			{
				std::shared_ptr<Client> user = userPair.first;
				int user_fd = user->getFd();
				auto it = std::find(fds_sent_to.begin(), fds_sent_to.end(), user_fd);
				if (user != client_ptr && it == fds_sent_to.end()) // Don't send the message to the sender
				{
					server_->send_response(user_fd, RPL_QUIT(":" + client_ptr->getNickname(), reason));
					fds_sent_to.push_back(user_fd);
				}
			}
			channel->removeUser(client_ptr);
		}
	}
	server_->disconnectAndDeleteClient(client_ptr);
}

