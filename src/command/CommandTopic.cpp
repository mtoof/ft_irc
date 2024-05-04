#include "Command.h"

void Command::handleTopic(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.empty())
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "TOPIC"));
		return;
	}

	std::string channelName = parameters[0];
	std::shared_ptr<Channel> channel = server_->findChannel(channelName);
	std::pair<std::string, std::string> topic = channel->getTopic();
	std::string currentTopic = topic.second;
	std::string currentTopicSetter = topic.first;

	if (!channel)
	{
		server_->send_response(fd, ERR_NOSUCHCHANNEL(channelName));
		return;
	}

	if (!channel->isUserOnChannel(client_ptr->getNickname()))
	{
		server_->send_response(fd, ERR_NOTONCHANNEL(channelName));
		return;
	}

	if (parameters.size() > 1)
	{
		std::string topic = parameters[1];

		if (topic.empty())
		{ // Request to clear the topic
			if (!channel->canChangeTopic(client_ptr))
			{
				server_->send_response(fd, ERR_CHANOPRIVSNEEDED(channelName));
				return;
			}
			channel->setTopic(make_pair(client_ptr->getNickname(), ""));
			server_->send_response(fd, RPL_TOPIC(client_ptr->getNickname(), channelName, "Topic cleared"));
		}
		else
		{ // Request to set a new topic
			if (!channel->canChangeTopic(client_ptr))
			{
				server_->send_response(fd, ERR_CHANOPRIVSNEEDED(channelName));
				return;
			}
			channel->setTopic(make_pair(client_ptr->getNickname(), topic));
			server_->send_response(fd, RPL_TOPIC(client_ptr->getNickname(), channelName, topic));
		}
	}
	else
	{
		if (currentTopic.empty())
			server_->send_response(fd, RPL_NOTOPIC(client_ptr->getNickname(), channelName));
		else
			server_->send_response(fd, RPL_TOPIC(client_ptr->getNickname(), channelName, currentTopic));
	}
}
