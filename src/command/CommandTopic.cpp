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

	std::string channel_name = parameters[0];
	std::shared_ptr<Channel> channel_ptr = server_->findChannel(channel_name);
	std::pair<std::string, std::string> topic;

	if (!channel_ptr)
	{
		server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
		return;
	}

	if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
	{
		server_->send_response(fd, ERR_NOTONCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_name));
		return;
	}

	topic = channel_ptr->getTopic();
	std::string current_topic = topic.second;
	std::string current_topic_setter = topic.first;
	std::string new_topic = msg.getTrailer();
	if (new_topic.size() > TOPIC_MAX_LENGTH) // topic is too long
		new_topic = new_topic.substr(0, TOPIC_MAX_LENGTH);
	if (parameters.size() && !new_topic.empty())
	{

		// if (new_topic == " ")
		// { // Request to clear the topic
		// 	if (channel_ptr->getModeT() && !channel_ptr->userIsOperator(client_ptr->getNickname()))
		// 	{
		// 		server_->send_response(fd, ERR_CHANOPRIVSNEEDED(server_->getServerHostname(), channel_name));
		// 		return;
		// 	}
		// 	channel_ptr->setTopic(make_pair(client_ptr->getNickname(), ""));
		// 	channel_ptr->broadcastMessageToAll(RPL_TOPIC(":" + client_ptr->getClientPrefix(), channel_name, "Topic cleared"));
		// 	//must be broadcast to everyone
		// }
		// else
		// { // Request to set a new topic
			if (channel_ptr->getModeT() && !channel_ptr->userIsOperator(client_ptr->getNickname()))
			{
				server_->send_response(fd, ERR_CHANOPRIVSNEEDED(server_->getServerHostname(), channel_name));
				return;
			}
			channel_ptr->setTopic(make_pair(client_ptr->getNickname(), new_topic));
			channel_ptr->broadcastMessageToAll(RPL_TOPIC(server_->getServerHostname(), client_ptr->getNickname(), channel_name, new_topic));
			//moj89 changed the topic of #test22 to: "new topic"
			//Topic for #test22: "new topic"
			//21:29 -!- Topic set by moj89 [~sepehr@da1c-55f3-94dc-acbc-ee7.bb.dnainternet.fi] [Tue May  7 21:24:26 2024]
			//must be broadcast to everyone
		
	}
	else
	{
		if (current_topic.empty())
			server_->send_response(client_ptr->getFd(), RPL_NOTOPIC(server_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName()));
		else
			server_->send_response(client_ptr->getFd(), RPL_TOPIC(server_->getServerHostname(), client_ptr->getNickname(), channel_name, current_topic));
	}
}

