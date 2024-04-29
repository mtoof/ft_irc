#include "Server.h"

void Server::shutdownServer(const std::string &reason)
{
	std::cout << RED << reason << ", shutting down server..." RESET << std::endl;
}

void Server::signalHandler(int signum)
{
	switch (signum)
	{
	case SIGINT:
		shutdownServer("SIGINT (Interrupt signal)");
		break;
	case SIGQUIT:
		shutdownServer("SIGQUIT (Quit)");
		break;
	}
	signal_ = true;
}

void Server::deleteClient(int fd)
{
	auto iter = clients_.find(fd);
	if (iter != clients_.end())
		clients_.erase(iter);
}

void Server::closeDeletePollFd(int fd)
{
	for (auto index = fds_.begin(); index != fds_.end(); index++)
	{
		if (index->fd == fd)
		{
			close(index->fd);
			fds_.erase(index);
			break;
		}
	}
}

void Server::closeFds()
{
	std::cout << RED "Closing all connections" RESET << std::endl;
	if (fds_.size() > 1)
	{
		for (auto index = fds_.begin(); index != fds_.end(); index++)
		{
			if (index->fd)
			{
				close(index->fd);
				fds_.erase(index);
			}
		}
	}
}

std::shared_ptr<Client> Server::findClientUsingFd(int fd) const
{
	std::cout << "clients_ size = " << clients_.size() << std::endl;
	if (clients_.empty())
		return nullptr;
	auto iter = clients_.find(fd);
	if (iter != clients_.end())
		return iter->second;
	return nullptr;
}

std::shared_ptr<Client> Server::findClientUsingNickname(std::string const &nickname) const
{
	std::cout << "findClientUsingNickname called" << std::endl;
	std::cout << "clients_ size = " << clients_.size() << std::endl;
	if (clients_.empty())
		return nullptr;
	for (auto it = clients_.begin(); it != clients_.end(); it++)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return nullptr;
}



void Server::whoGotDisconnected(int fd)
{
	std::shared_ptr<Client> client = findClientUsingFd(fd);
	std::string clientidentity;
	if (client)
	{
		if ((clientidentity = client->getNickname()).empty())
			clientidentity = std::to_string(fd);
	}
}

char *Server::extractUserIpAddress(struct sockaddr_in6 usersocketaddress)
{
	char *ipstr = nullptr;

	if (usersocketaddress.sin6_family == AF_INET6)
	{
		if (IN6_IS_ADDR_V4MAPPED(&(usersocketaddress.sin6_addr)))
		{
			// It's an IPv4-mapped IPv6 address, extract the IPv4 address
			struct in_addr ipv4addr;
			memcpy(&ipv4addr, &(usersocketaddress.sin6_addr.s6_addr[12]), sizeof(struct in_addr));
			ipstr = new char[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &ipv4addr, ipstr, INET_ADDRSTRLEN);
		}
		else
		{
			// It's a regular IPv6 address

			ipstr = new char[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, &(usersocketaddress.sin6_addr), ipstr, INET6_ADDRSTRLEN);
		}
	}
	else if (usersocketaddress.sin6_family == AF_INET)
	{
		// It's an IPv4 address
		ipstr = new char[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(usersocketaddress.sin6_addr), ipstr, INET_ADDRSTRLEN);
	}
	return ipstr;
}

void Server::send_response(int fd, const std::string &response)
{
	std::cout << "Response:\n" << response;
	if (send(fd, response.c_str(), response.length(), 0) < 0)
		debug("Response send() faild", FAILED);

}

// getter for map of supported commands
std::map<std::string, void (Command::*)(const Message &msg)> const &Server::getSupportedCommands() const
{
	return supported_commands_;
}

void Server::setServerHostname()
{
	char hostname[256] = {};
	if (gethostname(hostname, 256) == 0)
		host_ = hostname;
	else
		debug("gethostname", FAILED);
	return;
}

const std::string &Server::getServerHostname() const
{
	return host_;
}

/// @brief because user needs to feel welcome, they need to receive a welcome message
/// @param fd 
/// @param servername 
/// @param nickname 
/// @param client_prefix 
void Server::welcomeAndMOTD(int fd, std::string const &servername, std::string const &nickname, std::string const &client_prefix)
{
	send_response(fd, RPL_CONNECTED(servername, nickname, client_prefix));
	send_response(fd, RPL_MOTDSTART(servername, nickname));
	send_response(fd, RPL_MOTD(servername, nickname, "███████╗████████╗░░░░░░██╗██████╗░░█████╗░"));
	send_response(fd, RPL_MOTD(servername, nickname, "██╔════╝╚══██╔══╝░░░░░░██║██╔══██╗██╔══██╗"));
	send_response(fd, RPL_MOTD(servername, nickname, "█████╗░░░░░██║░░░█████╗██║██████╔╝██║░░╚═╝"));
	send_response(fd, RPL_MOTD(servername, nickname, "██╔══╝░░░░░██║░░░╚════╝██║██╔══██╗██║░░██╗"));
	send_response(fd, RPL_MOTD(servername, nickname, "██║░░░░░░░░██║░░░░░░░░░██║██║░░██║╚█████╔╝"));
	send_response(fd, RPL_MOTD(servername, nickname, "╚═╝░░░░░░░░╚═╝░░░░░░░░░╚═╝╚═╝░░╚═╝░╚════╝░"));
	send_response(fd, RPL_MOTD(servername, nickname, " "));
	send_response(fd, RPL_MOTD(servername, nickname, "░█████╗░░█████╗░███╗░░██╗██████╗░███████╗██╗░░░░░██╗███╗░░██╗"));
	send_response(fd, RPL_MOTD(servername, nickname, "██╔══██╗██╔══██╗████╗░██║██╔══██╗██╔════╝██║░░░░░██║████╗░██║"));
	send_response(fd, RPL_MOTD(servername, nickname, "██║░░██║███████║██╔██╗██║██║░░██║█████╗░░██║░░░░░██║██╔██╗██║"));
	send_response(fd, RPL_MOTD(servername, nickname, "██║░░██║██╔══██║██║╚████║██║░░██║██╔══╝░░██║░░░░░██║██║╚████║"));
	send_response(fd, RPL_MOTD(servername, nickname, "╚█████╔╝██║░░██║██║░╚███║██████╔╝███████╗███████╗██║██║░╚███║"));
	send_response(fd, RPL_MOTD(servername, nickname, "░╚════╝░╚═╝░░╚═╝╚═╝░░╚══╝╚═════╝░╚══════╝╚══════╝╚═╝╚═╝░░╚══╝"));
	send_response(fd, RPL_MOTD(servername, nickname, " "));
	send_response(fd, RPL_MOTD(servername, nickname, "███╗░░░███╗████████╗░█████╗░░█████╗░███████╗"));
	send_response(fd, RPL_MOTD(servername, nickname, "████╗░████║╚══██╔══╝██╔══██╗██╔══██╗██╔════╝"));
	send_response(fd, RPL_MOTD(servername, nickname, "██╔████╔██║░░░██║░░░██║░░██║██║░░██║█████╗░░"));
	send_response(fd, RPL_MOTD(servername, nickname, "██║╚██╔╝██║░░░██║░░░██║░░██║██║░░██║██╔══╝░░"));
	send_response(fd, RPL_MOTD(servername, nickname, "██║░╚═╝░██║░░░██║░░░╚█████╔╝╚█████╔╝██║░░░░░"));
	send_response(fd, RPL_MOTD(servername, nickname, "╚═╝░░░░░╚═╝░░░╚═╝░░░░╚════╝░░╚════╝░╚═╝░░░░░"));
	send_response(fd, RPL_MOTD(servername, nickname, " "));
	send_response(fd, RPL_MOTD(servername, nickname, "░█████╗░████████╗░█████╗░░█████╗░███████╗"));
	send_response(fd, RPL_MOTD(servername, nickname, "██╔══██╗╚══██╔══╝██╔══██╗██╔══██╗██╔════╝"));
	send_response(fd, RPL_MOTD(servername, nickname, "███████║░░░██║░░░██║░░██║██║░░██║█████╗░░"));
	send_response(fd, RPL_MOTD(servername, nickname, "██╔══██║░░░██║░░░██║░░██║██║░░██║██╔══╝░░"));
	send_response(fd, RPL_MOTD(servername, nickname, "██║░░██║░░░██║░░░╚█████╔╝╚█████╔╝██║░░░░░"));
	send_response(fd, RPL_MOTD(servername, nickname, "╚═╝░░╚═╝░░░╚═╝░░░░╚════╝░░╚════╝░╚═╝░░░░░"));
	send_response(fd, RPL_MOTD(servername, nickname, " "));
	send_response(fd, RPL_MOTD(servername, nickname, "\"Alright, let's see what we can see.. Everybody online, looking good.\""));
	send_response(fd, RPL_MOTD(servername, nickname, "Lieutenant Gorman to the marines before landing to terraforming colony on exomoon LV-426"));
	send_response(fd, RPL_MOTD(servername, nickname, "Aliens, 1986"));
	send_response(fd, RPL_MOTDEND(servername, nickname));
}