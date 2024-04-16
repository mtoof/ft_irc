#include "Server.h"

void Server::shutdownServer(const std::string& reason)
{
	std::cout << ", shutting down server...\n";
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
	for (auto index = clients_.begin(); index != clients_.end(); index++)
	{
		std::shared_ptr<Client> clientPtr = *index;
		if (clientPtr->getFd() == fd)
		{
			clients_.erase(index);
			break;
		}
	}
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
