/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atoof <atoof@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 12:14:19 by atoof             #+#    #+#             */
/*   Updated: 2024/04/15 12:14:20 by atoof            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./server/Server.h"
#include <iostream>


void convertArgs(int &port, string &password, char **av)
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
	if (av[2])
		password = av[2];
}

int main(int ac, char **av)
{
	// Server server;
	int port=-1;
	string password;
	try
	{
		switch (ac)
		{
			case 1:
				port = DEFAULTPORT;
				break;
			case 2:
			case 3:
				convertArgs(port, password, av);
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
	Server server(port, password);
	return 0;
}
