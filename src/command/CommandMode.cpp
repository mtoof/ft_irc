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

void Command::applyChannelModes(std::shared_ptr<Channel> channel, const std::string &mode_string, const std::string &mode_arguments, int fd) {
    std::vector<std::string> args;
    std::istringstream arg_stream(mode_arguments);
    std::string arg;
    while (arg_stream >> arg) {
        args.push_back(arg);
    }

    std::shared_ptr<Client> client_ptr = server_->findClientUsingFd(fd);
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
    for (const auto &arg : args) {
        params += arg + " ";
    }

    for (const auto &[mode, isSetting] : modeChanges) {
        if (isSetting) {
            switch (mode) {
            case 'k':
                if (arg_index < args.size()) {
                    channel->setModeK(true);
                    channel->setChannelKey(args[arg_index]);
                    modesToSet += mode;
                    arg_index++;  // Increment arg_index as this parameter has been used
                }
                break;
            case 'l':
                if (arg_index < args.size() && args[arg_index].find_first_not_of("0123456789") == std::string::npos) {
                    int limit = std::stoi(args[arg_index]);
                    channel->setModeL(true, limit);
                    modesToSet += mode;
                    arg_index++;  // Increment arg_index as this parameter has been used
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
                break;
            }
        } else {
            switch (mode) {
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
        std::string response = RPL_CHANGEMODE(client_ptr->getClientPrefix(), channel->getName(), modeResult, params);
        server_->send_response(fd, response);
        channel->broadcastMessage(client_ptr, response);
    }
}
