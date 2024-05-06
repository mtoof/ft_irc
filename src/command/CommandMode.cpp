#include "Command.h"

static std::string extractModeArguments(std::vector<std::string> &parameters)
{
	std::string mode_arguments = parameters[2];
	for (size_t i = 3; i < parameters.size(); i++) {
		mode_arguments += " " + parameters[i];
	}
	return mode_arguments;
}

static std::string getChannelModes(std::shared_ptr<Channel> channel_ptr)
{
	std::string modes = "+";
	if (channel_ptr->getModeI())
		modes += "i";
	if (channel_ptr->getModeL())
		modes += "l";
	if (channel_ptr->getModeT())
		modes += "t";
	if (channel_ptr->getModeK())
		modes += "k";
	return modes;
}

void Command::handleMode(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector <std::string> parameters = msg.getParameters();
	std::string target, mode_string, mode_arguments;
	if (parameters.empty())
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "MODE"));
		return;
	}
}
