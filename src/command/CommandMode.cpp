#include "Command.h"

/**
 * Extracts additional mode parameters from the command line input starting from the given index.
 *
 * @param mode_arguments Vector to store extracted mode arguments.
 * @param parameters Vector containing all command parameters.
 * @param start_index Index to start extraction from.
 */
static void extractModeArguments(std::vector<std::string> &mode_arguments, const std::vector<std::string> &parameters, size_t start_index)
{
    for (size_t i = start_index; i < parameters.size(); ++i)
        mode_arguments.push_back(parameters[i]);
}

/**
 * Retrieves the current modes set on a channel.
 *
 * @param channel_ptr Shared pointer to the channel object.
 * @param user_on_channel Determines whether user sending the command is on said channel
 * @return A string representing the modes set on the channel.
 */
static std::string getChannelModes(std::shared_ptr<Channel> channel_ptr, bool user_on_channel)
{
    std::string channel_modes = "+";
	std::string mode_params = "";
    if (channel_ptr->getModeN()) channel_modes += "n";
    if (channel_ptr->getModeI()) channel_modes += "i";
    if (channel_ptr->getModeT()) channel_modes += "t";
    if (channel_ptr->getModeK())
	{
		channel_modes += "k";
		if (user_on_channel) // only show the key if user is on channel
			mode_params += " " + channel_ptr->getChannelKey();
	}
    if (channel_ptr->getModeL())
	{
		channel_modes += "l";
		mode_params += " " + std::to_string(channel_ptr->getChannelLimit());
	}
    return channel_modes + mode_params;
}

/**
 * Handles the MODE command, determining whether to apply modes to a user or a channel.
 *
 * @param msg The message containing the command details.
 */
void Command::handleMode(const Message &msg)
{
    auto client_ptr = msg.getClientPtr();
    int client_fd = client_ptr->getFd();
    const auto &parameters = msg.getParameters();

    if (parameters.empty())
    {
        server_ptr_->send_response(client_fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), "MODE"));
        return;
    }

    const std::string &target = parameters[0];
    std::string mode_string = parameters.size() > 1 ? parameters[1] : "";
    std::vector<std::string> mode_arguments;
    if (parameters.size() > 2)
        extractModeArguments(mode_arguments, parameters, 2);

    if (target.front() != '#' && target.front() != '&')
        handleUserMode(client_ptr, target, mode_string);
    else
        handleChannelMode(client_ptr, target, mode_string, mode_arguments);
}

/**
 * Handles mode changes for a user.
 *
 * @param client Shared pointer to the client object.
 * @param target The target user for the mode change.
 * @param mode_string The string representing the modes to be applied.
 */
void Command::handleUserMode(std::shared_ptr<Client> client_ptr, const std::string &target, const std::string &mode_string)
{
    int client_fd = client_ptr->getFd();

    if (target == client_ptr->getNickname())
        applyUserMode(client_ptr, mode_string);
    else if (server_ptr_->findClientUsingNickname(target))
        server_ptr_->send_response(client_fd, ERR_USERSDONTMATCH(server_ptr_->getServerHostname(), client_ptr->getNickname()));
    else
        server_ptr_->send_response(client_fd, ERR_NOSUCHNICK(server_ptr_->getServerHostname(), client_ptr->getNickname(), target));
}

/**
 * Handles mode changes for a channel.
 *
 * @param client Shared pointer to the client object.
 * @param target The target channel for the mode change.
 * @param mode_string The string representing the modes to be applied.
 * @param mode_arguments The arguments associated with the modes.
 */
void Command::handleChannelMode(std::shared_ptr<Client> client_ptr, const std::string &target, const std::string &mode_string, const std::vector<std::string> &mode_arguments)
{
    auto channel_ptr = server_ptr_->findChannel(target);
    int client_fd = client_ptr->getFd();

    if (!channel_ptr)
        server_ptr_->send_response(client_fd, ERR_NOSUCHCHANNEL(server_ptr_->getServerHostname(), client_ptr->getNickname(), target));
    else if (mode_string.empty())
        server_ptr_->send_response(client_fd, RPL_CHANNELMODEIS(server_ptr_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName(), getChannelModes(channel_ptr, channel_ptr->isUserOnChannel(client_ptr->getNickname()))));
	else if (mode_string == "b")
		return;
    else if (!channel_ptr->isUserOnChannel(client_ptr->getNickname()))
        server_ptr_->send_response(client_fd, ERR_NOTONCHANNEL(server_ptr_->getServerHostname(), client_ptr->getNickname(), channel_ptr->getName()));
    else if (!channel_ptr->isOperator(client_ptr))
        server_ptr_->send_response(client_fd, ERR_CHANOPRIVSNEEDED(server_ptr_->getServerHostname(), channel_ptr->getName()));
    else
        applyChannelModes(client_ptr, channel_ptr, mode_string, mode_arguments);
}

/**
 * Applies mode changes to a user.
 *
 * @param client_ptr Shared pointer to the client object.
 * @param mode_string The string representing the modes to be applied.
 */
void Command::applyUserMode(std::shared_ptr<Client> client_ptr, const std::string &mode_string)
{
    int client_fd = client_ptr->getFd();
    std::string modeChange;

    if (!mode_string.empty())
    {
        bool isSettingMode = true;
        char last_sign = '\0';

        for (char mode : mode_string)
        {
            if (mode == '-')
                isSettingMode = false;
            else if (mode == '+')
                isSettingMode = true;
            else if (mode == 'i')
            {
                client_ptr->setModeI(isSettingMode);
                appendModeChange(modeChange, last_sign, isSettingMode, mode);
            }
            else
                server_ptr_->send_response(client_fd, ERR_UMODEUNKNOWNFLAG(server_ptr_->getServerHostname(), client_ptr->getNickname(), mode));
        }
        server_ptr_->send_response(client_fd, RPL_UMODECHANGE(client_ptr->getNickname(), modeChange));
    }
    else
        sendCurrentUserModes(client_ptr);
}

/**
 * Appends mode changes to the mode change string.
 *
 * @param modeChange The string representing the current mode changes.
 * @param last_sign The last mode character processed.
 * @param isSettingMode Boolean indicating if the mode is being set or unset.
 * @param mode The mode character being processed.
 */
void Command::appendModeChange(std::string &modeChange, char &last_sign, bool isSettingMode, char mode)
{
    if (isSettingMode && last_sign != '+')
        modeChange += '+';
    if (!isSettingMode && last_sign != '-')
        modeChange += '-';
    modeChange += mode;
    last_sign = isSettingMode ? '+' : '-';
}

/**
 * Sends the current modes set on a user to the client.
 *
 * @param client_ptr Shared pointer to the client object.
 */
void Command::sendCurrentUserModes(std::shared_ptr<Client> client_ptr)
{
    int client_fd = client_ptr->getFd();
    std::string currentUserModes = "+";
    if (client_ptr->getModeI())
        currentUserModes += "i";
    if (client_ptr->getModeLocalOp())
        currentUserModes += "O";
    server_ptr_->send_response(client_fd, RPL_UMODEIS(server_ptr_->getServerHostname(), client_ptr->getNickname(), currentUserModes));
}

/**
 * Applies mode changes to a channel.
 *
 * @param client Shared pointer to the client object.
 * @param channel Shared pointer to the channel object.
 * @param mode_string The string representing the modes to be applied.
 * @param modeArguments The arguments associated with the modes.
 */
void Command::applyChannelModes(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, const std::string &mode_string, const std::vector<std::string> &modeArguments)
{
    int client_fd = client->getFd();
    bool isSettingMode = true;
    std::string changedModes, usedParameters;
    size_t argumentIndex = 0;
    std::vector<std::pair<char, bool>> modeChanges;

    parseModeString(mode_string, modeChanges, isSettingMode);

    char last_sign = '\0';
    std::string processedModes;

    for (const auto &[modeChar, setMode] : modeChanges)
    {
        if (processedModes.find(modeChar) != std::string::npos)
            continue;
        handleModeChange(client, channel, modeChar, setMode, modeArguments, argumentIndex, changedModes, usedParameters, last_sign);
        processedModes += modeChar;
    }

    if (!changedModes.empty())
    {
        std::string finalResponse = RPL_CHANGEMODE(client->getClientPrefix(), channel->getName(), changedModes, usedParameters);
        server_ptr_->send_response(client_fd, finalResponse);
        channel->broadcastMessage(client, finalResponse, server_ptr_);
    }
}

/**
 * Parses a mode string and determines the mode changes.
 *
 * @param mode_string The string representing the modes to be applied.
 * @param modeChanges Vector to store the mode changes.
 * @param isSettingMode Boolean indicating if the mode is being set or unset.
 */
void Command::parseModeString(const std::string &mode_string, std::vector<std::pair<char, bool>> &modeChanges, bool &isSettingMode)
{
    for (char mode : mode_string)
    {
        if (mode == '+')
            isSettingMode = true;
        else if (mode == '-')
            isSettingMode = false;
        else
            modeChanges.emplace_back(mode, isSettingMode);
    }
}

/**
 * Handles a specific mode change for a channel.
 *
 * @param client Shared pointer to the client object.
 * @param channel Shared pointer to the channel object.
 * @param modeChar The mode character being processed.
 * @param setMode Boolean indicating if the mode is being set or unset.
 * @param modeArguments The arguments associated with the modes.
 * @param argumentIndex The current index in the mode arguments.
 * @param changedModes The string representing the current mode changes.
 * @param usedParameters The string representing the used parameters.
 * @param last_sign The last mode character processed.
 */
void Command::handleModeChange(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, char modeChar, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &last_sign)
{
    int client_fd = client->getFd();
    bool requiresParam = modeRequiresParameter(modeChar);
    bool mandatoryParam = mandatoryModeParameter(modeChar);

    if (requiresParam && setMode && argumentIndex >= modeArguments.size())
    {
        if (mandatoryParam)
            server_ptr_->send_response(client_fd, ERR_INVALIDMODEPARAM(server_ptr_->getServerHostname(), client->getNickname(), channel->getName(), modeChar, "", "Mode parameter missing."));
        return;
    }

    switch (modeChar)
    {
    case 'i':
        channel->setModeI(setMode);
        appendToChangedModeString(setMode, changedModes, last_sign, modeChar);
        break;
    case 'k':
        handleModeK(channel, setMode, modeArguments, argumentIndex, changedModes, usedParameters, last_sign);
        break;
    case 'l':
        handleModeL(channel, setMode, modeArguments, argumentIndex, changedModes, usedParameters, last_sign);
        break;
    case 'n':
        channel->setModeN(setMode);
        appendToChangedModeString(setMode, changedModes, last_sign, modeChar);
        break;
    case 't':
        channel->setModeT(setMode);
        appendToChangedModeString(setMode, changedModes, last_sign, modeChar);
        break;
    case 'o':
        handleModeO(client, channel, modeArguments, setMode, argumentIndex, changedModes, usedParameters, last_sign);
        break;
    default:
        server_ptr_->send_response(client_fd, ERR_UNKNOWNMODE(server_ptr_->getServerHostname(), client->getNickname(), modeChar));
        break;
    }
}

/**
 * Handles the 'k' (key) mode change for a channel.
 *
 * @param channel Shared pointer to the channel object.
 * @param setMode Boolean indicating if the mode is being set or unset.
 * @param modeArguments The arguments associated with the modes.
 * @param argumentIndex The current index in the mode arguments.
 * @param changedModes The string representing the current mode changes.
 * @param usedParameters The string representing the used parameters.
 * @param last_sign The last mode character processed.
 */
void Command::handleModeK(std::shared_ptr<Channel> channel, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &last_sign)
{
    if (setMode)
    {
        if (argumentIndex < modeArguments.size())
        {
            channel->setModeK(true);
            channel->setChannelKey(modeArguments[argumentIndex]);
            appendToChangedModeString(setMode, changedModes, last_sign, 'k');
            if (!usedParameters.empty())
                usedParameters += " ";
            usedParameters += modeArguments[argumentIndex];
            argumentIndex++;
        }
    }
    else
    {
        channel->setModeK(false);
        channel->setChannelKey("");
        appendToChangedModeString(setMode, changedModes, last_sign, 'k');
    }
}

/**
 * Handles the 'l' (limit) mode change for a channel.
 *
 * @param channel Shared pointer to the channel object.
 * @param setMode Boolean indicating if the mode is being set or unset.
 * @param modeArguments The arguments associated with the modes.
 * @param argumentIndex The current index in the mode arguments.
 * @param changedModes The string representing the current mode changes.
 * @param usedParameters The string representing the used parameters.
 * @param last_sign The last mode character processed.
 */
void Command::handleModeL(std::shared_ptr<Channel> channel, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &last_sign)
{
    if (setMode && argumentIndex < modeArguments.size())
    {
        if (modeArguments[argumentIndex].find_first_not_of("0123456789") == std::string::npos)
        {
            int limit = std::stoi(modeArguments[argumentIndex]);
            channel->setModeL(true, limit);
            appendToChangedModeString(setMode, changedModes, last_sign, 'l');
            if (!usedParameters.empty())
                usedParameters += " ";
            usedParameters += std::to_string(limit);
            argumentIndex++;
        }
    }
    else
    {
        channel->setModeL(false, 0);
        appendToChangedModeString(setMode, changedModes, last_sign, 'l');
    }
}

/**
 * Handles the 'o' (operator) mode change for a channel.
 *
 * @param client Shared pointer to the client object.
 * @param channel Shared pointer to the channel object.
 * @param modeArguments The arguments associated with the modes.
 * @param setMode Boolean indicating if the mode is being set or unset.
 * @param argumentIndex The current index in the mode arguments.
 * @param changedModes The string representing the current mode changes.
 * @param usedParameters The string representing the used parameters.
 * @param last_sign The last mode character processed.
 */
void Command::handleModeO(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, const std::vector<std::string> &modeArguments, bool setMode, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &last_sign)
{
    if (argumentIndex < modeArguments.size())
    {
        if (applyModeO(client, channel, modeArguments[argumentIndex], setMode))
        {
            appendToChangedModeString(setMode, changedModes, last_sign, 'o');
            if (!usedParameters.empty())
                usedParameters += " ";
            usedParameters += modeArguments[argumentIndex];
            argumentIndex++;
        }
    }
}

/**
 * Determines if a mode requires a parameter.
 *
 * @param mode The mode character.
 * @return True if the mode requires a parameter, false otherwise.
 */
bool Command::modeRequiresParameter(char mode)
{
    return mode == 'k' || mode == 'l' || mode == 'o';
}

/**
 * Determines if a mode parameter is mandatory.
 *
 * @param mode The mode character.
 * @return True if the mode parameter is mandatory, false otherwise.
 */
bool Command::mandatoryModeParameter(char mode)
{
    return mode == 'l';
}

/**
 * Applies the operator mode change to a channel.
 *
 * @param client Shared pointer to the client object.
 * @param channel Shared pointer to the channel object.
 * @param targetNickname The nickname of the target user.
 * @param setMode Boolean indicating if the mode is being set or unset.
 * @return True if the mode change was successful, false otherwise.
 */
bool Command::applyModeO(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, const std::string &targetNickname, bool setMode)
{
    auto targetClient = server_ptr_->findClientUsingNickname(targetNickname);
    int client_fd = client->getFd();

    if (!targetClient)
    {
        server_ptr_->send_response(client_fd, ERR_NOSUCHNICK(server_ptr_->getServerHostname(), client->getNickname(), targetNickname));
        return false;
    }

    if (!channel->isUserOnChannel(targetNickname))
    {
        server_ptr_->send_response(client_fd, ERR_USERNOTINCHANNEL(server_ptr_->getServerHostname(), client->getNickname(), targetNickname, channel->getName()));
        return false;
    }

    return channel->changeOpStatus(targetClient, setMode);
}

/**
 * Appends the mode change to the changed modes string.
 *
 * @param setMode Boolean indicating if the mode is being set or unset.
 * @param changedModes The string representing the current mode changes.
 * @param last_sign The last mode character processed.
 * @param modeChar The mode character being processed.
 */
void Command::appendToChangedModeString(bool setMode, std::string &changedModes, char &last_sign, char modeChar)
{
    if (setMode && last_sign != '+')
        changedModes += '+';
    if (!setMode && last_sign != '-')
        changedModes += '-';
    changedModes += modeChar;
    last_sign = setMode ? '+' : '-';
}