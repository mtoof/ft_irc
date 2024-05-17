#include "Command.h"

/**
 * Extracts additional mode parameters from the command line input starting from the given index.
 *
 * @param modeArguments Vector to store extracted mode arguments.
 * @param parameters Vector containing all command parameters.
 * @param startIndex Index to start extraction from.
 */
static void extractModeArguments(std::vector<std::string> &modeArguments, const std::vector<std::string> &parameters, size_t startIndex)
{
    for (size_t i = startIndex; i < parameters.size(); ++i)
        modeArguments.push_back(parameters[i]);
}

/**
 * Retrieves the current modes set on a channel.
 *
 * @param channel Shared pointer to the channel object.
 * @return A string representing the modes set on the channel.
 */
static std::string getChannelModes(std::shared_ptr<Channel> channel)
{
    std::string modes = "+";
    if (channel->getModeN()) modes += "n";
    if (channel->getModeI()) modes += "i";
    if (channel->getModeT()) modes += "t";
    if (channel->getModeL()) modes += "l";
    if (channel->getModeK()) modes += "k";
    return modes;
}

/**
 * Handles the MODE command, determining whether to apply modes to a user or a channel.
 *
 * @param msg The message containing the command details.
 */
void Command::handleMode(const Message &msg)
{
    auto client = msg.getClientPtr();
    int clientFd = client->getFd();
    const auto &parameters = msg.getParameters();

    if (parameters.empty())
    {
        server_->send_response(clientFd, ERR_NEEDMOREPARAMS(client->getClientPrefix(), "MODE"));
        return;
    }

    const std::string &target = parameters[0];
    std::string modeString = parameters.size() > 1 ? parameters[1] : "";
    std::vector<std::string> modeArguments;
    if (parameters.size() > 2)
        extractModeArguments(modeArguments, parameters, 2);

    if (target.front() != '#' && target.front() != '&')
        handleUserMode(client, target, modeString);
    else
        handleChannelMode(client, target, modeString, modeArguments);
}

/**
 * Handles mode changes for a user.
 *
 * @param client Shared pointer to the client object.
 * @param target The target user for the mode change.
 * @param modeString The string representing the modes to be applied.
 */
void Command::handleUserMode(std::shared_ptr<Client> client, const std::string &target, const std::string &modeString)
{
    int clientFd = client->getFd();

    if (target == client->getNickname())
        applyUserMode(client, modeString);
    else if (server_->findClientUsingNickname(target))
        server_->send_response(clientFd, ERR_USERSDONTMATCH(server_->getServerHostname(), client->getNickname()));
    else
        server_->send_response(clientFd, ERR_NOSUCHNICK(server_->getServerHostname(), client->getNickname(), target));
}

/**
 * Handles mode changes for a channel.
 *
 * @param client Shared pointer to the client object.
 * @param target The target channel for the mode change.
 * @param modeString The string representing the modes to be applied.
 * @param modeArguments The arguments associated with the modes.
 */
void Command::handleChannelMode(std::shared_ptr<Client> client, const std::string &target, const std::string &modeString, const std::vector<std::string> &modeArguments)
{
    auto channel = server_->findChannel(target);
    int clientFd = client->getFd();

    if (!channel)
        server_->send_response(clientFd, ERR_NOSUCHCHANNEL(server_->getServerHostname(), client->getNickname(), target));
    else if (modeString.empty())
        server_->send_response(clientFd, RPL_CHANNELMODEIS(server_->getServerHostname(), client->getNickname(), channel->getName(), getChannelModes(channel)));
    else if (!channel->isUserOnChannel(client->getNickname()))
        server_->send_response(clientFd, ERR_NOTONCHANNEL(server_->getServerHostname(), client->getNickname(), channel->getName()));
    else if (!channel->isOperator(client))
        server_->send_response(clientFd, ERR_CHANOPRIVSNEEDED(server_->getServerHostname(), channel->getName()));
    else
        applyChannelModes(client, channel, modeString, modeArguments);
}

/**
 * Applies mode changes to a user.
 *
 * @param client Shared pointer to the client object.
 * @param modeString The string representing the modes to be applied.
 */
void Command::applyUserMode(std::shared_ptr<Client> client, const std::string &modeString)
{
    int clientFd = client->getFd();
    std::string modeChange;

    if (!modeString.empty())
    {
        bool isSettingMode = true;
        char lastModeChar = '\0';

        for (char mode : modeString)
        {
            if (mode == '-')
                isSettingMode = false;
            else if (mode == '+')
                isSettingMode = true;
            else if (mode == 'i')
            {
                client->setModeI(isSettingMode);
                appendModeChange(modeChange, lastModeChar, isSettingMode, mode);
            }
            else
                server_->send_response(clientFd, ERR_UMODEUNKNOWNFLAG(server_->getServerHostname(), client->getNickname(), mode));
        }
        server_->send_response(clientFd, RPL_UMODECHANGE(client->getNickname(), modeChange));
    }
    else
        sendCurrentUserModes(client);
}

/**
 * Appends mode changes to the mode change string.
 *
 * @param modeChange The string representing the current mode changes.
 * @param lastModeChar The last mode character processed.
 * @param isSettingMode Boolean indicating if the mode is being set or unset.
 * @param mode The mode character being processed.
 */
void Command::appendModeChange(std::string &modeChange, char &lastModeChar, bool isSettingMode, char mode)
{
    if (isSettingMode && lastModeChar != '+')
        modeChange += '+';
    if (!isSettingMode && lastModeChar != '-')
        modeChange += '-';
    modeChange += mode;
    lastModeChar = isSettingMode ? '+' : '-';
}

/**
 * Sends the current modes set on a user to the client.
 *
 * @param client Shared pointer to the client object.
 */
void Command::sendCurrentUserModes(std::shared_ptr<Client> client)
{
    int clientFd = client->getFd();
    std::string currentUserModes = "+";
    if (client->getModeI())
        currentUserModes += "i";
    if (client->getModeLocalOp())
        currentUserModes += "O";
    server_->send_response(clientFd, RPL_UMODEIS(server_->getServerHostname(), client->getNickname(), currentUserModes));
}

/**
 * Applies mode changes to a channel.
 *
 * @param client Shared pointer to the client object.
 * @param channel Shared pointer to the channel object.
 * @param modeString The string representing the modes to be applied.
 * @param modeArguments The arguments associated with the modes.
 */
void Command::applyChannelModes(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, const std::string &modeString, const std::vector<std::string> &modeArguments)
{
    int clientFd = client->getFd();
    bool isSettingMode = true;
    std::string changedModes, usedParameters;
    size_t argumentIndex = 0;
    std::vector<std::pair<char, bool>> modeChanges;

    parseModeString(modeString, modeChanges, isSettingMode);

    char lastModeChar = '\0';
    std::string processedModes;

    for (const auto &[modeChar, setMode] : modeChanges)
    {
        if (processedModes.find(modeChar) != std::string::npos)
            continue;
        handleModeChange(client, channel, modeChar, setMode, modeArguments, argumentIndex, changedModes, usedParameters, lastModeChar);
        processedModes += modeChar;
    }

    if (!changedModes.empty())
    {
        std::string finalResponse = RPL_CHANGEMODE(client->getClientPrefix(), channel->getName(), changedModes, usedParameters);
        server_->send_response(clientFd, finalResponse);
        channel->broadcastMessage(client, finalResponse, server_);
    }
}

/**
 * Parses a mode string and determines the mode changes.
 *
 * @param modeString The string representing the modes to be applied.
 * @param modeChanges Vector to store the mode changes.
 * @param isSettingMode Boolean indicating if the mode is being set or unset.
 */
void Command::parseModeString(const std::string &modeString, std::vector<std::pair<char, bool>> &modeChanges, bool &isSettingMode)
{
    for (char mode : modeString)
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
 * @param lastModeChar The last mode character processed.
 */
void Command::handleModeChange(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, char modeChar, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &lastModeChar)
{
    int clientFd = client->getFd();
    bool requiresParam = modeRequiresParameter(modeChar);
    bool mandatoryParam = mandatoryModeParameter(modeChar);

    if (requiresParam && setMode && argumentIndex >= modeArguments.size())
    {
        if (mandatoryParam)
            server_->send_response(clientFd, ERR_INVALIDMODEPARAM(server_->getServerHostname(), client->getNickname(), channel->getName(), modeChar, "", "Mode parameter missing."));
        return;
    }

    switch (modeChar)
    {
    case 'i':
        channel->setModeI(setMode);
        appendToChangedModeString(setMode, changedModes, lastModeChar, modeChar);
        break;
    case 'k':
        handleModeK(channel, setMode, modeArguments, argumentIndex, changedModes, usedParameters, lastModeChar);
        break;
    case 'l':
        handleModeL(channel, setMode, modeArguments, argumentIndex, changedModes, usedParameters, lastModeChar);
        break;
    case 'n':
        channel->setModeN(setMode);
        appendToChangedModeString(setMode, changedModes, lastModeChar, modeChar);
        break;
    case 't':
        channel->setModeT(setMode);
        appendToChangedModeString(setMode, changedModes, lastModeChar, modeChar);
        break;
    case 'o':
        handleModeO(client, channel, modeArguments, setMode, argumentIndex, changedModes, usedParameters, lastModeChar);
        break;
    default:
        server_->send_response(clientFd, ERR_UNKNOWNMODE(server_->getServerHostname(), client->getNickname(), modeChar));
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
 * @param lastModeChar The last mode character processed.
 */
void Command::handleModeK(std::shared_ptr<Channel> channel, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &lastModeChar)
{
    if (setMode)
    {
        if (argumentIndex < modeArguments.size())
        {
            channel->setModeK(true);
            channel->setChannelKey(modeArguments[argumentIndex]);
            appendToChangedModeString(setMode, changedModes, lastModeChar, 'k');
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
        appendToChangedModeString(setMode, changedModes, lastModeChar, 'k');
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
 * @param lastModeChar The last mode character processed.
 */
void Command::handleModeL(std::shared_ptr<Channel> channel, bool setMode, const std::vector<std::string> &modeArguments, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &lastModeChar)
{
    if (setMode && argumentIndex < modeArguments.size())
    {
        if (modeArguments[argumentIndex].find_first_not_of("0123456789") == std::string::npos)
        {
            int limit = std::stoi(modeArguments[argumentIndex]);
            channel->setModeL(true, limit);
            appendToChangedModeString(setMode, changedModes, lastModeChar, 'l');
            if (!usedParameters.empty())
                usedParameters += " ";
            usedParameters += std::to_string(limit);
            argumentIndex++;
        }
    }
    else
    {
        channel->setModeL(false, 0);
        appendToChangedModeString(setMode, changedModes, lastModeChar, 'l');
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
 * @param lastModeChar The last mode character processed.
 */
void Command::handleModeO(std::shared_ptr<Client> client, std::shared_ptr<Channel> channel, const std::vector<std::string> &modeArguments, bool setMode, size_t &argumentIndex, std::string &changedModes, std::string &usedParameters, char &lastModeChar)
{
    if (argumentIndex < modeArguments.size())
    {
        if (applyModeO(client, channel, modeArguments[argumentIndex], setMode))
        {
            appendToChangedModeString(setMode, changedModes, lastModeChar, 'o');
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
    auto targetClient = server_->findClientUsingNickname(targetNickname);
    int clientFd = client->getFd();

    if (!targetClient)
    {
        server_->send_response(clientFd, ERR_NOSUCHNICK(server_->getServerHostname(), client->getNickname(), targetNickname));
        return false;
    }

    if (!channel->isUserOnChannel(targetNickname))
    {
        server_->send_response(clientFd, ERR_USERNOTINCHANNEL(server_->getServerHostname(), client->getNickname(), targetNickname, channel->getName()));
        return false;
    }

    return channel->changeOpStatus(targetClient, setMode);
}

/**
 * Appends the mode change to the changed modes string.
 *
 * @param setMode Boolean indicating if the mode is being set or unset.
 * @param changedModes The string representing the current mode changes.
 * @param lastModeChar The last mode character processed.
 * @param modeChar The mode character being processed.
 */
void Command::appendToChangedModeString(bool setMode, std::string &changedModes, char &lastModeChar, char modeChar)
{
    if (setMode && lastModeChar != '+')
        changedModes += '+';
    if (!setMode && lastModeChar != '-')
        changedModes += '-';
    changedModes += modeChar;
    lastModeChar = setMode ? '+' : '-';
}