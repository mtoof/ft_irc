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
			server_->send_response(fd, ERR_UNKNOWNMODE(client_ptr->getNickname(), params[0], mode_string[i]));
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
	target = parameters[0];
	if (parameters.size() > 1)
		mode_string = parameters[1];
	if (parameters.size() > 2)
		mode_arguments = extractModeArguments(parameters);
	if (target.front() != '#' && target.front() != '&')
	{
		if (target == client_ptr->getNickname())
		{
			if (!mode_string.empty())
			{
				if (mode_string == "+i")
				{
					client_ptr->setUserMode(mode_string.at(1));
				}
				// what user modes do we want to handle?
			}
			server_->send_response(fd, RPL_UMODEIS(client_ptr->getNickname(), client_ptr->getUserMode()));
		}
		else if (server_->findClientUsingNickname(target) != nullptr)
		{
			server_->send_response(fd, ERR_USERSDONTMATCH(server_->getServerHostname(), client_ptr->getNickname()));
		}
		else
			server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), target));
	}
	else
	{
		std::cout << "target: " << target << std::endl;
		std::shared_ptr<Channel> channel_ptr = server_->findChannel(target);
		if (!channel_ptr)
			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), target));
		else
		{
			if (mode_string.empty()) // if the command is just MODE #channel, then we list the channel modes.
				server_->send_response(fd, RPL_CHANNELMODEIS(server_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName(), getChannelModes(channel_ptr)));
			else
			{
				if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
				{
					server_->send_response(fd, ERR_NOTONCHANNEL(target));
					return;
				}
				if (!channel_ptr->isOperator(client_ptr))
				{
					server_->send_response(fd, ERR_NOTOPERATOR(target));
					return;
				}
				bool mode = true; // if there is no + in the beginning, it is assumed mode is added
				for(size_t i = 0; i < mode_string.size(); i++)
				{
					switch(mode_string[i])
					{
						case '+':
							mode = true;
							break;
						case '-':
							mode = false;
							break;
						case 't':
							if (channel_ptr->getModeT() != mode)
								channel_ptr->setModeT(mode);
							break;
						case 'i':
							if (channel_ptr->getModeI() != mode)
								channel_ptr->setModeI(mode);
							break;
						// TODO: modes k, l and o need to be handled,
						// but then you need to also go through mode_arguments in the same order
						// if you send MODE +tikl 42 password, the password will be set to 42, limit won't be set/changed.
						default:
							server_->send_response(fd, ERR_UNKNOWNMODE(client_ptr->getNickname(), channel_ptr->getName(), mode_string[i]));
							break;
					}
				}
				// TODO:
				// the mode string in the response below should be the same as the actually changed modes
				// maybe do a separate string changed_modes which will be updated as we go
				server_->send_response(fd, RPL_CHANGEMODE(client_ptr->getClientPrefix(), channel_ptr->getName(), mode_string, ""));
				return;
				}
			}
		}
	}


