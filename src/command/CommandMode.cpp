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

void Command::extractMode(const Message &msg, const std::vector<std::string> &params, std::string &mode_string)
{
	std::string supported_modes = "itkol";
	std::string extracred_modes;
	mode_string = params[1];
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	for (size_t i = 0; i < mode_string.size(); i++)
	{
		if (mode_string[i] == '+' || mode_string[i] == '-' || supported_modes.find(mode_string[i]) != std::string::npos)
			extracred_modes += mode_string[i];
		else
		{
			server_->send_response(fd, ERR_UNKNOWNMODE(client_ptr->getNickname().c_str(), params[1].c_str() , std::to_string(mode_string[i])));
		}
	}
}

void Command::handleMode(const Message &msg)
{
	std::regex mode_regex("^[+-]?[a-zA-Z]+$");
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector <std::string> parameters = msg.getParameters();
	std::string target, mode_string, mode_arguments;
	switch (parameters.size())
	{
		case 0:
			server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "MODE"));
			break;
		case 1:
			target = parameters[0];
			break;
		case 2:
			target = parameters[0];
			break;
		default:
			target = parameters[0];
			extractMode(msg, parameters, mode_string);
			for (int index = 1; index < parameters.size(); index++)
			{
				mode_string += parameters[index];
			}
			break;
	}
	std::shared_ptr<Channel> channel_ptr = server_->findChannel(target);
	if (channel_ptr)
	{

	}
	else
	{
		server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), target));
		return;
	}
}
