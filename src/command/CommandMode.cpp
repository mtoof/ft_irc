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
	std::map<char, bool> modeChanges; // Tracks the final setting state for each mode
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

	bool hasRelevantModes = false; // Tracks whether there are any modes that require additional parameters
	std::string relevantParams = ""; // Contains the parameters for the relevant modes

	for (const auto &[mode, isSetting] : modeChanges) // Iterate over the modes and apply them
	{
		if (isSetting) // if the mode is being set
		{
			switch (mode)
			{
			case 'k':
				if (arg_index < args.size())
				{
					channel->setModeK(true);
					channel->setChannelKey(args[arg_index]);
					modesToSet += mode;
					relevantParams += args[arg_index++] + " ";
					hasRelevantModes = true;
				}
				break;
			case 'l':
				if (arg_index < args.size())
				{
					if (args[arg_index].find_first_not_of("0123456789") != std::string::npos)
					{
						server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "MODE"));
						break;
					}
					int limit = std::stoi(args[arg_index]);
					channel->setModeL(true, limit);
					modesToSet += mode;
					relevantParams += args[arg_index++] + " ";
					hasRelevantModes = true;
				}
				break;
			case 'i':
			case 't':
				if (mode == 'i')
					channel->setModeI(true);
				if (mode == 't')
					channel->setModeT(true);
				modesToSet += mode;
				break;
			default:
				server_->send_response(fd, ERR_UNKNOWNMODE(server_->getServerHostname(), client_ptr->getNickname(), mode));
				break;
			}
		}
		else
		{
			if (mode == 'k' || mode == 'l' || mode == 'i' || mode == 't')
			{
				modesToUnset += mode;
				if (mode == 'k' || mode == 'l')
				{
					channel->setModeK(false);
					channel->setChannelKey("");
					channel->setModeL(false, 0);
					hasRelevantModes = true;
				}
			}
		}
	}

	std::string modeResult = (!modesToSet.empty() ? "+" + modesToSet : "") + (!modesToUnset.empty() ? "-" + modesToUnset : "");
	if (!modeResult.empty())
	{
		std::string response = RPL_CHANGEMODE(client_ptr->getClientPrefix(), channel->getName(), modeResult, (hasRelevantModes ? relevantParams : std::string("")));
		server_->send_response(fd, response);
		channel->broadcastMessage(client_ptr, response);
	}
}
