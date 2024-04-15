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
