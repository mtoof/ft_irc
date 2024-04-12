// #include <Server.hpp>
#include <iostream>

#define DEFAULTPORT 6667

void convertArgs(int &port, char **av)
{
	int index;
	for (index = 0; av[1][index];index++)
	{
		if (!std::isdigit(av[1][index]))
			throw std::runtime_error("Port must be a digit");
	}
	port = std::stoi(av[1]);
	if (port < 0 || (port >= 0 && port <= 1024) || port > 65536)
		throw std::runtime_error("Invalid port number, must be between 1025 and 65536");
}

int main(int ac, char **av)
{
	// Server server;
	int port;
	try
	{
		switch (ac)
		{
			case 1:
				port = DEFAULTPORT;
				break;
			case 2:
			case 3:
				convertArgs(port, av);
				break;
			default:
				std::cout << "Usage:\n./ircserv <Port> <Password>";
				break ;
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return (1);
	}
	return 0;
}