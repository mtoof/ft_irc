#include "Server.h"

void Server::shutdownServer(const std::string& reason)
{
	std::cout << RED ", shutting down server..." RESET << std::endl;
	(void)reason;
}

void Server::signalHandler(int signum)
{
	switch (signum) {
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

std::shared_ptr<Client>	Server::findClientUsingFd(int fd) const
{
	std::shared_ptr <Client> client = std::make_shared<Client>();
	auto iter = clients_.find(fd);
	if (iter != clients_.end())
		return iter->second;
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

int Server::extractUserIpAddress(struct sockaddr_in6 usersocketaddress, std::shared_ptr<Client> &newclient)
{
	char ipv4str[INET_ADDRSTRLEN];
	char ipv6str[INET6_ADDRSTRLEN];

	if (usersocketaddress.sin6_family == AF_INET6)
	{
		if (IN6_IS_ADDR_V4MAPPED(&(usersocketaddress.sin6_addr)))
		{
			// It's an IPv4-mapped IPv6 address, extract the IPv4 address
			struct in_addr ipv4addr;
			memcpy(&ipv4addr, &(usersocketaddress.sin6_addr.s6_addr[12]), sizeof(struct in_addr));
			inet_ntop(AF_INET, &ipv4addr, ipv4str, INET_ADDRSTRLEN);
			newclient->setIpAddress(ipv4str);
			std::cout << "Client IPv4: " << ipv4str << std::endl;
		} else {
			// It's a regular IPv6 address
			inet_ntop(AF_INET6, &(usersocketaddress.sin6_addr), ipv6str, INET6_ADDRSTRLEN);
			newclient->setIpAddress(ipv6str);
			std::cout << "Client IPv6: " << ipv6str << std::endl;
		}
	} else if (usersocketaddress.sin6_family == AF_INET) {
		// It's an IPv4 address
		inet_ntop(AF_INET, &(usersocketaddress.sin6_addr), ipv4str, INET_ADDRSTRLEN);
		newclient->setIpAddress(ipv4str);
		std::cout << "Client IPv4: " << ipv4str << std::endl;
	} else {
		return (-1);
	}
	return 0;
}