#ifndef __REPLY_H__
#define __REPLY_H__

#define CRLF "\r\n"
#define CLIENT(nickname, username, IPaddr) (":" + nickname + "!~" + username + "@" + IPaddr)
#define PONG(servername, token) (":" + servername + " PONG " + servername + " :" + token + CRLF)

// REPLIES

#define RPL_PRIVMSG(CLIENT, target, text) (CLIENT + " PRIVMSG " + target + " " + text + CRLF)
#define RPL_NICKCHANGECHANNEL(old_prefix, nickname) (":" + old_prefix + " NICK :" + nickname + CRLF)
#define RPL_CONNECTED(servername, nickname, client_prefix) (":" + servername + " 001 " + nickname + " :Welcome to the best ever IRC server! " + client_prefix + CRLF)
#define RPL_NICKCHANGE(oldprefix, nickname) (":" + oldprefix + " NICK " + nickname + CRLF)
#define RPL_UMODEIS(NICK, modes) (NICK + " " + modes + CRLF)
#define RPL_CREATIONTIME(nickname, channelname, creationtime) (": 329 " + nickname + " #" + channelname + " " + creationtime + CRLF)
#define RPL_CHANNELMODES(nickname, channelname, modes) (": 324 " + nickname + " #" + channelname + " " + modes + CRLF)
#define RPL_CHANGEMODE(hostname, channelname, mode, arguments) (":" + hostname + " MODE #" + channelname + " " + mode + " " + arguments + CRLF)
#define RPL_JOINMSG(clientprefix, channelname) (":" + clientprefix + " JOIN " + channelname + CRLF)
#define RPL_NAMREPLY(servername, nickname, channelname, clientslist) (":" + servername + " 353 " + nickname + " = " + channelname + " :" + clientslist + CRLF)
#define RPL_ENDOFNAMES(servername, nickname, channelname) (":" + servername + " 366 " + nickname + " " + channelname + " :END of /NAMES list" + CRLF)
#define RPL_TOPICIS(nickname, channelname, topic) (": 332 " + nickname + " " + channelname + " :" + topic + CRLF)
#define RPL_INVITING(nickname, channelname, invited) ("341 " + nickname + " " + invited + " " + channelname + CRLF)
#define RPL_INVITED(CLIENT, nickname, channelname) (CLIENT + " INVITE " + nickname + " " + channelname + CRLF)
#define RPL_WHOISUSER(servername, nickname, username, hostname, realname) (":" + servername + " 311 " + nickname + " " + username + " " + hostname + " * :" + realname + CRLF)
#define RPL_ENDOFWHOIS(servername, nickname) (":" + servername + " 318 " + nickname + " :End of WHOIS list." + CRLF)
#define RPL_NOTOPIC(CLIENT, channelname) (CLIENT + " TOPIC " + channelname + " :" + CRLF)
#define RPL_TOPIC(CLIENT, channelname, topic) (CLIENT + " TOPIC " + channelname + " " + topic + CRLF)
#define RPL_YOUREOPER(CLIENT, channel, nickname) (CLIENT + " MODE " + channel + " +o " + nickname + CRLF)
#define RPL_YOURENOTOPER(CLIENT, channel, nickname) (CLIENT + " MODE " + channel + " -o " + nickname + CRLF)
#define RPL_KICK(CLIENT, channel, nickname, msg) (CLIENT + " KICK " + channel + " " + nickname + " " + msg + CRLF)
#define RPL_QUIT(CLIENT, msg) (CLIENT + " QUIT " + msg + CRLF)
#define RPL_MOTDSTART(servername, nickname)(":" + servername + " 375 " + nickname + " :- " + servername + " Message of the day -" + CRLF)
#define RPL_MOTD(servername, nickname, message)(":" + servername + " 372 " + nickname + " :- " + message + CRLF)
#define RPL_MOTDEND(servername, nickname)(":" + servername + " 376 " + nickname + " :End of MOTD command" + CRLF)

// ERRORS

#define ERR_NEEDMOREPARAMS(client_prefix, command) (":" + client_prefix + " 461 " + command + " :Not enough parameters given." + CRLF)
#define ERR_NOTREGISTERED(servername) (":" + servername + " 451 " + "*" + " :You have not registered." + CRLF)
#define ERR_NONICKNAMEGIVEN(client_prefix) (client_prefix + " 431 :No nickname given" + CRLF)
#define ERR_NICKINUSE(servername, nickname) (":" + servername + " 433 * " + nickname + " :Nickname is already in use" + CRLF)
#define ERR_ERRONEUSNICK(servername, nickname, bad_nickname) (":" + servername + " 432 " + nickname + " " + bad_nickname + " :Erroneus nickname" + CRLF)
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :You are already registered!" + CRLF)
#define ERR_INCORPASS(nickname) (": 464 " + nickname + " :Password incorrect! try again!" + CRLF)
#define ERR_NEEDMODEPARM(channelname, mode) (": 696 #" + channelname + " * You must specify a parameter for the key mode. " + mode + CRLF)
#define ERR_INVALIDMODEPARM(channelname, mode) (": 696 #" + channelname + " Invalid mode parameter. " + mode + CRLF)
#define ERR_KEYSET(channelname) (": 467 #" + channelname + " Channel key already set. " + CRLF)
#define ERR_UNKNOWNMODE(nickname, channelname, mode) (": 472 " + nickname + " #" + channelname + " " + mode + " :is not a recognised channel mode" + CRLF)
#define ERR_CHANNELNOTFOUND(nickname, channelname) (": 403 " + nickname + " " + channelname + " :No such channel" + CRLF)
#define ERR_NOTOPERATOR(channelname) (": 482 #" + channelname + " :You're not a channel operator" + CRLF)
#define ERR_NOSUCHCHANNEL(channel) ("403 * " + channel + " :No such channel" + CRLF)
#define ERR_CMDNOTFOUND(nickname, command) (": 421 " + nickname + " " + command + " :Unknown command" + CRLF)
#define ERR_NOTONCHANNEL(channel) ("442 " + channel + " :You're not on that channel" + CRLF)
#define ERR_INVITEONLYCHAN(hostname, nickname, channel) (":" + hostname + " 473 " + nickname + " " + channel + " :Cannot join channel (+i)" + CRLF)
#define ERR_BADCHANNELKEY(channel) ("475 " + channel + " :Cannot join channel (+k)" + CRLF)
#define ERR_CHANNELISFULL(channel) ("471 " + channel + " :Cannot join channel (+l)" + CRLF)
#define ERR_USERONCHANNEL(hostname, invited, channel) (":" + hostname + " " + invited + " " + channel + " :is already on channel" + CRLF)
#define ERR_CHANOPRIVSNEEDED(channel) ("482 " + channel + " :You're not a channel operator" + CRLF)
#define ERR_NOSUCHNICK(nickname) (": 401 " + nickname + " :No such nick/channel" + CRLF)

#endif
