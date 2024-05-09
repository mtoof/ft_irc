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
	std::vector<std::string> parameters = msg.getParameters();

	if (parameters.size() < 1)
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "MODE"));
		return;
	}

	std::string target = parameters[0];
	std::string mode_string = parameters.size() > 1 ? parameters[1] : "";
	std::string mode_arguments = parameters.size() > 2 ? extractModeArguments(parameters, 2) : "";

	if (target.front() != '#' && target.front() != '&')
	{
		if (target == client_ptr->getNickname())
			applyUserMode(client_ptr, mode_string);
		else if (server_->findClientUsingNickname(target) != nullptr)
			server_->send_response(fd, ERR_USERSDONTMATCH(server_->getServerHostname(), client_ptr->getNickname()));
		else
			server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), target));
	}
	else 
	{
		std::shared_ptr<Channel> channel_ptr = server_->findChannel(target);
		if (!channel_ptr)
		{
			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), target));
			return;
		}
		else if (mode_string.empty()) // if the command is just MODE #channel, then we list the channel modes.
		{
			server_->send_response(fd, RPL_CHANNELMODEIS(server_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName(), getChannelModes(channel_ptr)));
		}
		else if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
		{
			server_->send_response(fd, ERR_NOTONCHANNEL(channel_ptr->getName()));
		}
		else if (!channel_ptr->isOperator(client_ptr))
		{
			server_->send_response(fd, ERR_NOTOPERATOR(channel_ptr->getName()));
		}
		else
			applyChannelModes(client_ptr, channel_ptr, mode_string, mode_arguments);
	}
}

void Command::applyUserMode(std::shared_ptr<Client> client_ptr, std::string mode_string)
{
	int fd = client_ptr->getFd();
	std::string mode_change = "";
	if (!mode_string.empty())
	{
		bool mode = true;
		for (size_t i = 0; i < mode_string.size(); i++)
		{
			if (mode_string.at(i) == '-')
				mode = false;
			else if (mode_string.at(i) == '+')
				mode = true;
			else if (mode_string.at(i) == 'i')
			{
				client_ptr->setModeI(mode);
				if (mode == true)
					mode_change += "+";
				else
					mode_change += "-";
				mode_change += 'i';
			}
			else
				server_->send_response(fd, ERR_UMODEUNKNOWNFLAG(server_->getServerHostname(), client_ptr->getNickname(), mode_string.at(i)));
		}
		server_->send_response(fd, RPL_UMODECHANGE(client_ptr->getNickname(), mode_change));
	}
	else
	{
		std::string usermode = "+";
		if (client_ptr->getModeI() == true)
			usermode += "i";
		server_->send_response(fd, RPL_UMODEIS(server_->getServerHostname(), client_ptr->getNickname(), usermode));
	}
}

void Command::applyChannelModes(std::shared_ptr<Client> client_ptr, std::shared_ptr<Channel> channel_ptr, const std::string &mode_string, const std::string &mode_arguments) 
{
    std::vector<std::string> args;
    std::istringstream arg_stream(mode_arguments);
    std::string arg;
    while (arg_stream >> arg) {
        args.push_back(arg);
    }
	int fd = client_ptr->getFd();
    std::map<char, bool> modeChanges; // Tracks the final setting state for each mode
    std::string modesToSet = "";
    std::string modesToUnset = "";
    std::string params = "";  // This will hold all parameters for display
    size_t arg_index = 0;

    char lastSign = '+';
    for (char mode : mode_string) {
        if (mode == '+')
            lastSign = '+';
        else if (mode == '-')
            lastSign = '-';
        else
            modeChanges[mode] = (lastSign == '+');
    }

    // Collect all parameters in a single string for display regardless of modes
    // for (const auto &arg : args) {
    //     params += arg + " ";
    // }

    for (const auto &[mode, isSetting] : modeChanges) {
        if (isSetting) {
            switch (mode) {
            case 'k':
                if (arg_index < args.size()) {
                    channel_ptr->setModeK(true);
                    channel_ptr->setChannelKey(args[arg_index]);
                    modesToSet += mode;
					params += args[arg_index] + " ";
                    arg_index++;  // Increment arg_index as this parameter has been used
                }
                break;
            case 'l':
                if (arg_index < args.size() && args[arg_index].find_first_not_of("0123456789") == std::string::npos) {
                    int limit = std::stoi(args[arg_index]);
                    channel_ptr->setModeL(true, limit);
                    modesToSet += mode;
					params += args[arg_index] + " ";
                    arg_index++;  // Increment arg_index as this parameter has been used
                }
                break;
            case 'i':
                channel_ptr->setModeI(true);
                modesToSet += mode;
                break;
            case 'o':
				if (arg_index < args.size())
				{
					if (applyModeO(client_ptr, channel_ptr, args[arg_index], true))
					{
						modesToSet += mode;
						params += args[arg_index] + " ";
					}
					arg_index++;
				}
			 	break;
			case 't':
                channel_ptr->setModeT(true);
                modesToSet += mode;
                break;
			default:
                server_->send_response(fd, ERR_UNKNOWNMODE(server_->getServerHostname(), client_ptr->getNickname(), mode));
                break;
            }
        } else {
            switch (mode) {
            case 'k':
                channel_ptr->setModeK(false);
                channel_ptr->setChannelKey("");
                modesToUnset += mode;
                break;
            case 'l':
                channel_ptr->setModeL(false, 0);
                modesToUnset += mode;
                break;
            case 'i':
                channel_ptr->setModeI(false);
                modesToUnset += mode;
                break;
			case 'o':
				if (arg_index < args.size())
				{
					if (applyModeO(client_ptr, channel_ptr, args[arg_index], false))
					{
						modesToUnset += mode;
						params += args[arg_index] + " ";
					}
					arg_index++;
				}
				break;
            case 't':
                channel_ptr->setModeT(false);
                modesToUnset += mode;
                break;
            default:
                server_->send_response(fd, ERR_UNKNOWNMODE(server_->getServerHostname(), client_ptr->getNickname(), mode));
                break;
            }
        }
    }

    // Construct the mode change result string
    std::string modeResult;
    if (!modesToSet.empty()) modeResult += "+" + modesToSet;
    if (!modesToUnset.empty()) modeResult += "-" + modesToUnset;

    // Ensure the mode change output is followed by all parameters
    if (!modeResult.empty()) {
        std::string response = RPL_CHANGEMODE(client_ptr->getClientPrefix(), channel_ptr->getName(), modeResult, params);
        server_->send_response(fd, response);
        channel_ptr->broadcastMessage(client_ptr, response);
    }
}

bool Command::applyModeO(std::shared_ptr<Client> client_ptr, std::shared_ptr<Channel> channel_ptr, std::string target, bool mode)
{
	std::shared_ptr<Client> target_ptr = server_->findClientUsingNickname(target);
	if (target_ptr == nullptr)
	{
		server_->send_response(client_ptr->getFd(), ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), target));
		return false;
	}
	if (!channel_ptr->isUserOnChannel(target))
	{
		server_->send_response(client_ptr->getFd(), ERR_USERNOTINCHANNEL(client_ptr->getClientPrefix(), client_ptr->getNickname(), target, channel_ptr->getName()));
		return false;
	}
	return channel_ptr->changeOpStatus(target_ptr, mode);
}