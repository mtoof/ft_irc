#include "Command.h"

void Command::handleTopic(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	std::vector<std::string> parameters = msg.getParameters();
	int fd = client_ptr->getFd();
	if (parameters.empty())
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "TOPIC"));
		return;
	}

	std::string channelName = parameters[0];
	std::shared_ptr<Channel> channel = server_->findChannel(channelName);
	std::pair<std::string, std::string> topic;

	if (!channel)
	{
		server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channelName));
		return;
	}

	if (!channel->isUserOnChannel(client_ptr->getNickname()))
	{
		server_->send_response(fd, ERR_NOTONCHANNEL(channelName));
		return;
	}

	topic = channel->getTopic();
	std::string currentTopic = topic.second;
	std::string currentTopicSetter = topic.first;
	std::string user_topic = msg.getTrailer();
	if (parameters.size() && !user_topic.empty())
	{

		if (user_topic == " ")
		{ // Request to clear the topic
			if (!channel->canChangeTopic(client_ptr))
			{
				server_->send_response(fd, ERR_CHANOPRIVSNEEDED(channelName));
				return;
			}
			channel->setTopic(make_pair(client_ptr->getNickname(), ""));
			channel->broadcastMessageToAll(RPL_TOPIC(":" + client_ptr->getClientPrefix(), channelName, "Topic cleared"));
			//must be broadcast to everyone
		}
		else
		{ // Request to set a new topic
			if (!channel->canChangeTopic(client_ptr))
			{
				server_->send_response(fd, ERR_CHANOPRIVSNEEDED(channelName));
				return;
			}
			channel->setTopic(make_pair(client_ptr->getNickname(), user_topic));
			channel->broadcastMessageToAll(RPL_TOPIC(":" + client_ptr->getClientPrefix(), channelName, user_topic));
			//moj89 changed the topic of #test22 to: "new topic"
			//Topic for #test22: "new topic"
			//21:29 -!- Topic set by moj89 [~sepehr@da1c-55f3-94dc-acbc-ee7.bb.dnainternet.fi] [Tue May  7 21:24:26 2024]
			//must be broadcast to everyone
		}
	}
	else
	{
		if (currentTopic.empty())
			server_->send_response(client_ptr->getFd(), RPL_NOTOPIC(":"+client_ptr->getClientPrefix(), channelName));
		else
			server_->send_response(client_ptr->getFd(), RPL_TOPIC(":" + client_ptr->getClientPrefix(), channelName, currentTopic));
	}
}

