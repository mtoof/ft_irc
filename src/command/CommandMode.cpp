#include "Command.h"

// Extracts additional mode parameters from the command line input starting from the given index.
static void extractModeArguments(std::vector<std::string> &mode_arguments, const std::vector<std::string> &parameters, size_t start)
{
    for (size_t i = start; i < parameters.size(); ++i)
    {
        mode_arguments.push_back(parameters[i]);
    }
}

static std::string getChannelModes(std::shared_ptr<Channel> channel_ptr)
{
	std::string modes = "+";
	if (channel_ptr->getModeN())
		modes += "n";
	if (channel_ptr->getModeI())
		modes += "i";
	if (channel_ptr->getModeT())
		modes += "t";
	if (channel_ptr->getModeL())
		modes += "l";
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
    std::vector<std::string> mode_arguments;
	if (parameters.size() > 2)
		extractModeArguments(mode_arguments, parameters, 2);

    if (target.front() != '#' && target.front() != '&')
    {
        if (target == client_ptr->getNickname())
            applyUserMode(client_ptr, mode_string);
        else if (server_->findClientUsingNickname(target) != nullptr)
            server_->send_response(fd, ERR_USERSDONTMATCH(server_->getServerHostname(), client_ptr->getNickname()));
        else
            server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), target));
    }
    else // Channel mode change request received
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
            server_->send_response(fd, ERR_NOTONCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName()));
        }
        else if (!channel_ptr->isOperator(client_ptr))
        {
            server_->send_response(fd, ERR_CHANOPRIVSNEEDED(server_->getServerHostname(), channel_ptr->getName()));
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
        char last_mode_char = '\0';
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
                {
                    if (last_mode_char != '+')
                        mode_change += "+";
                    last_mode_char = '+';
                }
                else
                {
                    if (last_mode_char != '-')
                        mode_change += "-";
                    last_mode_char = '-';
                }
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

void Command::applyChannelModes(std::shared_ptr<Client> client_ptr, std::shared_ptr<Channel> channel_ptr, const std::string &mode_string, const std::vector<std::string> &mode_arguments)
{
	int fd = client_ptr->getFd();

	bool setting = true;
	std::string changed_modes = "";
	std::string used_params = "";

	std::vector<std::pair<char, bool>> mode_vector; // To store mode and whether it's setting or unsetting.
	
	for (size_t i = 0; i < mode_string.size(); i++)
    {
        char mode = mode_string[i];
        if (mode == '+')
            setting = true;
        else if (mode == '-')
            setting = false;
        else
			mode_vector.push_back({mode, setting}); // Store the mode change
    }

	char last_mode_char = '\0';
	size_t arg_index = 0;
	size_t mode_index = 0;
	std::string mode_chars;
	for (auto &[mode_char, plusmode] : mode_vector)
	{
		// if (mode_index > 0)
	 	// {
			// auto it = mode_vector.find(mode_vector.begin(), )
		if (mode_chars.find_first_of(mode_char) != std::string::npos)
				continue;
		// }	
		bool requires_param = modeRequiresParameter(mode_char);
        bool param_is_mandatory = mandatoryModeParameter(mode_char);

        // If a mode requires a parameter and it's missing, skip this mode
        if (requires_param && plusmode && arg_index >= mode_arguments.size())
        {
            if (param_is_mandatory && plusmode)
                server_->send_response(fd, ERR_INVALIDMODEPARAM(server_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName(), mode_char , "", "Mode parameter missing."));
            continue;
        }
		switch (mode_char)
		{
			case 'i':
				channel_ptr->setModeI(plusmode);
				appendToChangedModeString(plusmode, changed_modes, last_mode_char, mode_char);
				break;
			case 'k':
				if (plusmode)
				{
					if(arg_index < mode_arguments.size())
					{
						channel_ptr->setModeK(true);
						channel_ptr->setChannelKey(mode_arguments[arg_index]);
						appendToChangedModeString(plusmode, changed_modes, last_mode_char, mode_char);
						if (!used_params.empty())
							used_params += " ";
						used_params += mode_arguments[arg_index];
						arg_index++;
					}
				}
				else
				{
					channel_ptr->setModeK(false);
					channel_ptr->setChannelKey("");
					appendToChangedModeString(plusmode, changed_modes, last_mode_char, mode_char);
				}
				break;
			case 'l':
				if (plusmode && arg_index < mode_arguments.size())
                {
					if (mode_arguments[arg_index].find_first_not_of("0123456789") == std::string::npos)
					{
						int limit = std::stoi(mode_arguments[arg_index]);
						channel_ptr->setModeL(true, limit);
						appendToChangedModeString(plusmode, changed_modes, last_mode_char, mode_char);
						if (!used_params.empty())
							used_params += " ";
						used_params += std::to_string(limit);
					}
					arg_index++;
                }
				else
				{
					channel_ptr->setModeL(false, 0);
					appendToChangedModeString(plusmode, changed_modes, last_mode_char, mode_char);
				}
				break;
			case 'n':
				channel_ptr->setModeN(plusmode);
				appendToChangedModeString(plusmode, changed_modes, last_mode_char, mode_char);
				break;
			case 't':
				channel_ptr->setModeT(plusmode);
				appendToChangedModeString(plusmode, changed_modes, last_mode_char, mode_char);
				break;
			case 'o':
				if (arg_index < mode_arguments.size())
				{
					if (applyModeO(client_ptr, channel_ptr, mode_arguments[arg_index], plusmode))
					{
						appendToChangedModeString(plusmode, changed_modes, last_mode_char, mode_char);
						if (!used_params.empty())
							used_params += " ";
						used_params += mode_arguments[arg_index];
						arg_index++;
					}
				}
				break;
			default:
                server_->send_response(fd, ERR_UNKNOWNMODE(server_->getServerHostname(), client_ptr->getNickname(), mode_char));
                break;
		}
		mode_chars += mode_char;
		mode_index++;
	}
	if (!changed_modes.empty()) // If there are any mode changes, send the response
    {
        std::string final_response = RPL_CHANGEMODE(
            client_ptr->getClientPrefix(),
            channel_ptr->getName(),
            changed_modes,
            used_params);
        server_->send_response(fd, final_response);
        channel_ptr->broadcastMessage(client_ptr, final_response);
    }
}

bool Command::modeRequiresParameter(char mode)
{
    switch (mode)
    {
    case 'k':
    case 'l':
    case 'o':
        return true;
    default:
        return false;
    }
}

bool Command::mandatoryModeParameter(char mode)
{
    switch (mode)
    {
    case 'l':
        return true;
    default:
        return false;
    }
}

bool Command::applyModeO(std::shared_ptr<Client> client_ptr, std::shared_ptr<Channel> channel_ptr, std::string target, bool mode)
{
    std::shared_ptr<Client> target_ptr = server_->findClientUsingNickname(target);
	bool isTrue = true;
    if (target_ptr == nullptr)
    {
        server_->send_response(client_ptr->getFd(), ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), target));
		isTrue = false;
    }
	if (!channel_ptr->isUserOnChannel(target))
    {
        server_->send_response(client_ptr->getFd(), ERR_USERNOTINCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), target, channel_ptr->getName()));
		isTrue = false;
    }
    isTrue = channel_ptr->changeOpStatus(target_ptr, mode);
	return isTrue;
}

void Command::appendToChangedModeString(bool plusmode, std::string &changed_modes, char &last_mode_char, char &mode_char)
{
	if (plusmode && last_mode_char != '+')
		changed_modes += '+';
	else if (!plusmode && last_mode_char != '-')
		changed_modes += '-';
	changed_modes += mode_char;
	if (plusmode)
		last_mode_char = '+';
	else
		last_mode_char = '-';
}
// #include "Command.h"

// // Extracts additional mode parameters from the command line input starting from the given index.
// static std::string extractModeArguments(const std::vector<std::string> &parameters, size_t start)
// {
// 	std::string mode_arguments;
// 	for (size_t i = start; i < parameters.size(); ++i)
// 	{
// 		if (!mode_arguments.empty())
// 			mode_arguments += " ";
// 		mode_arguments += parameters[i];
// 	}
// 	return mode_arguments;
// }

// static std::string getChannelModes(std::shared_ptr<Channel> channel_ptr)
// {
// 	std::string modes = "+";
// 	if (channel_ptr->getModeI())
// 		modes += "i";
// 	if (channel_ptr->getModeL())
// 		modes += "l";
// 	if (channel_ptr->getModeT())
// 		modes += "t";
// 	if (channel_ptr->getModeK())
// 		modes += "k";
// 	return modes;
// }

// void Command::handleMode(const Message &msg)
// {
// 	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
// 	int fd = client_ptr->getFd();
// 	std::vector<std::string> parameters = msg.getParameters();

// 	if (parameters.size() < 1)
// 	{
// 		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "MODE"));
// 		return;
// 	}

// 	std::string target = parameters[0];
// 	std::string mode_string = parameters.size() > 1 ? parameters[1] : "";
// 	std::string mode_arguments = parameters.size() > 2 ? extractModeArguments(parameters, 2) : "";

// 	if (target.front() != '#' && target.front() != '&')
// 	{
// 		if (target == client_ptr->getNickname())
// 			applyUserMode(client_ptr, mode_string);
// 		else if (server_->findClientUsingNickname(target) != nullptr)
// 			server_->send_response(fd, ERR_USERSDONTMATCH(server_->getServerHostname(), client_ptr->getNickname()));
// 		else
// 			server_->send_response(fd, ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), target));
// 	}
// 	else 
// 	{
// 		std::shared_ptr<Channel> channel_ptr = server_->findChannel(target);
// 		if (!channel_ptr)
// 		{
// 			server_->send_response(fd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), target));
// 			return;
// 		}
// 		else if (mode_string.empty()) // if the command is just MODE #channel, then we list the channel modes.
// 		{
// 			server_->send_response(fd, RPL_CHANNELMODEIS(server_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName(), getChannelModes(channel_ptr)));
// 		}
// 		else if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
// 		{
// 			server_->send_response(fd, ERR_NOTONCHANNEL(server_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName()));
// 		}
// 		else if (!channel_ptr->isOperator(client_ptr))
// 		{
// 			server_->send_response(fd, ERR_CHANOPRIVSNEEDED(server_->getServerHostname(), channel_ptr->getName()));
// 		}
// 		else
// 			applyChannelModes(client_ptr, channel_ptr, mode_string, mode_arguments);
// 	}
// }

// void Command::applyUserMode(std::shared_ptr<Client> client_ptr, std::string mode_string)
// {
// 	int fd = client_ptr->getFd();
// 	std::string mode_change = "";
// 	if (!mode_string.empty())
// 	{
// 		bool mode = true;
// 		for (size_t i = 0; i < mode_string.size(); i++)
// 		{
// 			if (mode_string.at(i) == '-')
// 				mode = false;
// 			else if (mode_string.at(i) == '+')
// 				mode = true;
// 			else if (mode_string.at(i) == 'i')
// 			{
// 				client_ptr->setModeI(mode);
// 				if (mode == true)
// 					mode_change += "+";
// 				else
// 					mode_change += "-";
// 				mode_change += 'i';
// 			}
// 			else
// 				server_->send_response(fd, ERR_UMODEUNKNOWNFLAG(server_->getServerHostname(), client_ptr->getNickname(), mode_string.at(i)));
// 		}
// 		server_->send_response(fd, RPL_UMODECHANGE(client_ptr->getNickname(), mode_change));
// 	}
// 	else
// 	{
// 		std::string usermode = "+";
// 		if (client_ptr->getModeI() == true)
// 			usermode += "i";
// 		server_->send_response(fd, RPL_UMODEIS(server_->getServerHostname(), client_ptr->getNickname(), usermode));
// 	}
// }

// void Command::applyChannelModes(std::shared_ptr<Client> client_ptr, std::shared_ptr<Channel> channel_ptr, const std::string &mode_string, const std::string &mode_arguments) 
// {
//     std::vector<std::string> args;
//     std::istringstream arg_stream(mode_arguments);
//     std::string arg;
//     while (arg_stream >> arg) {
//         args.push_back(arg);
//     }
// 	int fd = client_ptr->getFd();
//     std::vector<std::pair <char, bool>> mode_changes; // Tracks the final setting state for each mode
//     std::string modesToSet = "";
//     std::string modesToUnset = "";
//     std::string params = "";  // This will hold all parameters for display
//     size_t arg_index = 0;

//     bool status = 'true';
//     for (char mode : mode_string) {
//         if (mode == '+')
//             status = true;
//         else if (mode == '-')
//         	status = false;
//         else
//             mode_changes.push_back(std::pair(mode, status));
//     }
	
//     // Collect all parameters in a single string for display regardless of modes
//     // for (const auto &arg : args) {
//     //     params += arg + " ";
//     // }
// 	for (const auto setting_pair : mode_changes) {
//         char mode = setting_pair.first;
// 		bool setting = setting_pair.second;
// 		if (setting) {
//             switch (mode) {
//             case 'k':
// 			    if (arg_index < args.size()) {
//                     channel_ptr->setModeK(true);
//                     channel_ptr->setChannelKey(args[arg_index]);
//                     modesToSet += mode;
// 					params += args[arg_index] + " ";
//                     arg_index++;  // Increment arg_index as this parameter has been used
//                 }
//                 break;
//             case 'l':
// 				if (arg_index < args.size() && args[arg_index].find_first_not_of("0123456789") == std::string::npos) {
//                     int limit = std::stoi(args[arg_index]);
//                     channel_ptr->setModeL(true, limit);
//                     modesToSet += mode;
// 					params += args[arg_index] + " ";
//                     arg_index++;  // Increment arg_index as this parameter has been used
//                 }
//                 break;
//             case 'i':
//                 channel_ptr->setModeI(true);
//                 modesToSet += mode;
//                 break;
//             case 'o':
// 				if (arg_index < args.size())
// 				{
// 					if (applyModeO(client_ptr, channel_ptr, args[arg_index], true))
// 					{
// 						modesToSet += mode;
// 						params += args[arg_index] + " ";
// 					}
// 					arg_index++;
// 				}
// 			 	break;
// 			case 't':
//                 channel_ptr->setModeT(true);
//                 modesToSet += mode;
//                 break;
// 			default:
//                 server_->send_response(fd, ERR_UNKNOWNMODE(server_->getServerHostname(), client_ptr->getNickname(), mode));
//                 break;
//             }
//         } else {
//             switch (mode) {
//             case 'k':
//                 channel_ptr->setModeK(false);
//                 channel_ptr->setChannelKey("");
//                 modesToUnset += mode;
//                 break;
//             case 'l':
//                 channel_ptr->setModeL(false, 0);
//                 modesToUnset += mode;
//                 break;
//             case 'i':
//                 channel_ptr->setModeI(false);
//                 modesToUnset += mode;
//                 break;
// 			case 'o':
// 				if (arg_index < args.size())
// 				{
// 					if (applyModeO(client_ptr, channel_ptr, args[arg_index], false))
// 					{
// 						modesToUnset += mode;
// 						params += args[arg_index] + " ";
// 					}
// 					arg_index++;
// 				}
// 				break;
//             case 't':
//                 channel_ptr->setModeT(false);
//                 modesToUnset += mode;
//                 break;
//             default:
//                 server_->send_response(fd, ERR_UNKNOWNMODE(server_->getServerHostname(), client_ptr->getNickname(), mode));
//                 break;
//             }
//         }
//     }

//     // Construct the mode change result string
// 	// TODO: not minus or plus first but in the exact order the changes have been done
//     std::string modeResult;
//     if (!modesToSet.empty()) modeResult += "+" + modesToSet;
//     if (!modesToUnset.empty()) modeResult += "-" + modesToUnset;

//     // Ensure the mode change output is followed by all parameters
//     if (!modeResult.empty()) {
//         std::string response = RPL_CHANGEMODE(client_ptr->getClientPrefix(), channel_ptr->getName(), modeResult, params);
//         server_->send_response(fd, response);
//         channel_ptr->broadcastMessage(client_ptr, response);
//     }
// }

// bool Command::applyModeO(std::shared_ptr<Client> client_ptr, std::shared_ptr<Channel> channel_ptr, std::string target, bool mode)
// {
// 	std::cout << target << std::endl;
// 	std::shared_ptr<Client> target_ptr = server_->findClientUsingNickname(target);
// 	if (target_ptr == nullptr)
// 	{
// 		server_->send_response(client_ptr->getFd(), ERR_NOSUCHNICK(server_->getServerHostname(), client_ptr->getNickname(), target));
// 		return false;
// 	}
// 	if (!channel_ptr->isUserOnChannel(target))
// 	{
// 		server_->send_response(client_ptr->getFd(), ERR_USERNOTINCHANNEL(client_ptr->getClientPrefix(), client_ptr->getNickname(), target, channel_ptr->getName()));
// 		return false;
// 	}
// 	return channel_ptr->changeOpStatus(target_ptr, mode);
// }