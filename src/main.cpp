/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atoof <atoof@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 20:06:13 by atoof             #+#    #+#             */
/*   Updated: 2024/04/02 20:06:13 by atoof            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool server_running = true;

static void shutdown_server(const std::string& reason) {
    std::cout << reason << ", shutting down server...\n";
    server_running = false;
}

static void signal_handler(int signum) {
    switch (signum) {
    case SIGINT:
        shutdown_server("SIGINT (Interrupt signal)");
        break;
    case SIGTERM:
        shutdown_server("SIGTERM (Termination signal)");
        break;
    case SIGQUIT:
        shutdown_server("SIGQUIT (Quit signal)");
        break;
    case SIGHUP:
        shutdown_server("SIGHUP (Hangup signal)");
        break;
    }
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port number> <password>\n";
		return EXIT_FAILURE;
	}
	int port;
	try
	{
		port = std::stoi(argv[1]);
	}
	catch (std::exception &e)
	{
		std::cerr << "Invalid port number\n";
		return EXIT_FAILURE;
	}
	std::string password = argv[2];
	if (password.length() < 1)
	{
		std::cerr << "Password must be at least one character long\n";
		return EXIT_FAILURE;
	}
	std::cout << "IRC server started on port \"" << port << "\" with password \"" << password << "\"" << std::endl;
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGHUP, signal_handler);
	// TODO: initialize the server with the given port and password
	Server server(port, password);
	return (0);
}
