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

// static void shutdown_server(const std::string& reason) {
//     std::cout << reason << ", shutting down server...\n";
//     server_running = false;
// }

// static void signal_handler(int signum) {
//     switch (signum) {
//     case SIGINT:
//         shutdown_server("SIGINT (Interrupt signal)");
//         break;
//     case SIGTERM:
//         shutdown_server("SIGTERM (Termination signal)");
//         break;
//     case SIGQUIT:
//         shutdown_server("SIGQUIT (Quit signal)");
//         break;
//     case SIGHUP:
//         shutdown_server("SIGHUP (Hangup signal)");
//         break;
//     }
// }

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
	// signal(SIGINT, signal_handler);
	// signal(SIGTERM, signal_handler);
	// signal(SIGQUIT, signal_handler);
	// signal(SIGHUP, signal_handler);
	// // TODO: initialize the server with the given port and password
	Server server(port, password);
	struct sockaddr_in	_socketAddr;
	int new_socket, serverFD;
	int addLen = sizeof(_socketAddr);
	// serverFD = socket(AF_INET, SOCK_STREAM, O_NONBLOCK);
	if ((serverFD = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
	if (fcntl(serverFD, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
	// setsockopt();
	_socketAddr.sin_family = AF_INET;
	_socketAddr.sin_addr.s_addr = INADDR_ANY;
	_socketAddr.sin_port = htons(6667);
	bind(serverFD, (struct sockaddr *)&_socketAddr, sizeof(_socketAddr));
	listen(serverFD, 3);
	// while (1)
	// {
		std::cout << "Waiting for connections...\n" << std::endl;
		if ((new_socket = accept(serverFD, (struct sockaddr *)&_socketAddr, (socklen_t*)&addLen))<0) {
            perror("accept");
            // continue; // If accept fails, continue to the next iteration of the loop
        }
		std::string message = "Hello from server\n";
        send(new_socket, message.c_str(), message.length(), 0);
        
        // Close the socket after handling it
        close(new_socket);
	// }
	close(serverFD);
	return (0);
}
