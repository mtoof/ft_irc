#include "Command.h"

// Extracts additional mode parameters from the command line input starting from the given index.
static std::string extractModeArguments(const std::vector<std::string> &parameters, size_t start)
{
	std::string mode_arguments;
	for (size_t i = start; i < parameters.size(); ++i)
	{
		if (!mode_arguments.empty())
			mode_arguments += " ";
		mode_arguments += parameters[i];
	}
	return mode_arguments;
}

// Returns a string representing the active modes of a channel.
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

// Filters and validates the mode string from a command, ensuring only supported modes are processed.
void Command::extractMode(const Message &msg, const std::vector<std::string> &params, std::string &mode_string)
{
	std::string supported_modes = getChannelModes(server_->findChannel(params[0]));
	std::string validated_modes;
	mode_string = params[1];
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::shared_ptr<Channel> channel_ptr = server_->findChannel(params[0]);

	for (char c : mode_string)
	{
		if (c == '+' || c == '-' || supported_modes.find(c) != std::string::npos)
			validated_modes += c;
		else
			server_->send_response(fd, ERR_UNKNOWNMODE(client_ptr->getNickname(), channel_ptr->getName(), c));
	}
	mode_string = validated_modes;
}

// Handles the MODE command, directing the processing and application of modes on channels.
void Command::handleMode(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	int fd = client_ptr->getFd();
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.size() < 2)
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "MODE"));
		return;
	}

	std::string target = parameters[0];
	std::string mode_string = parameters[1];
	std::string mode_arguments = parameters.size() > 2 ? extractModeArguments(parameters, 2) : "";

	std::shared_ptr<Channel> channel_ptr = server_->findChannel(target);
	if (!channel_ptr)
	{
		server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), target));
		return;
	}

	applyChannelModes(channel_ptr, mode_string, mode_arguments, fd);
}

// Applies extracted and validated modes to the specified channel.
void Command::applyChannelModes(std::shared_ptr<Channel> channel, const std::string &mode_string, const std::string &mode_arguments, int fd)
{
	std::vector<std::string> args;
	std::istringstream arg_stream(mode_arguments);
	std::string arg;
	while (arg_stream >> arg)
		args.push_back(arg);

	std::shared_ptr<Client> client_ptr = server_->findClientUsingFd(fd);
	std::map<char, bool> modeChanges; // Final state for each mode
	std::string modesToSet = "";
	std::string modesToUnset = "";
	std::string params = "";
	size_t arg_index = 0;

	char lastSign = '+';
	for (char mode : mode_string)
	{
		if (mode == '+')
			lastSign = '+';
		else if (mode == '-')
			lastSign = '-';
		else
			modeChanges[mode] = (lastSign == '+');
	}

	for (const auto &[mode, isSetting] : modeChanges)
	{
		if (isSetting)
		{
			switch (mode)
			{
			case 'k':
				if (arg_index < args.size())
				{
					channel->setModeK(true);
					channel->setChannelKey(args[arg_index]);
					modesToSet += mode;
					params += args[arg_index++] + " ";
				}
				break;
			case 'l':
				if (arg_index < args.size())
				{
					int limit = std::stoi(args[arg_index]);
					channel->setModeL(true, limit);
					modesToSet += mode;
					params += args[arg_index++] + " ";
				}
				break;
			case 'i':
				channel->setModeI(true);
				modesToSet += mode;
				break;
			case 't':
				channel->setModeT(true);
				modesToSet += mode;
				break;
			default:
				server_->send_response(fd, ERR_UNKNOWNMODE(server_->getServerHostname(), client_ptr->getNickname(), mode));
				break; // Ignore unknown modes
			}
		}
		else
		{
			switch (mode)
			{
			case 'k':
				channel->setModeK(false);
				channel->setChannelKey("");
				modesToUnset += mode;
				break;
			case 'l':
				channel->setModeL(false, 0);
				modesToUnset += mode;
				break;
			case 'i':
				channel->setModeI(false);
				modesToUnset += mode;
				break;
			case 't':
				channel->setModeT(false);
				modesToUnset += mode;
				break;
			default:
				server_->send_response(fd, ERR_UNKNOWNMODE(server_->getServerHostname(), client_ptr->getNickname(), mode));
				break; // Ignore unknown modes
			}
		}
	}

	std::string modeResult;
	if (!modesToSet.empty())
		modeResult += "+" + modesToSet;
	if (!modesToUnset.empty())
		modeResult += "-" + modesToUnset;

	if (!modeResult.empty())
	{
		std::string response = RPL_CHANGEMODE(client_ptr->getClientPrefix(), channel->getName(), modeResult, params);
		server_->send_response(fd, response);
		channel->broadcastMessage(client_ptr, response);
	}
}
