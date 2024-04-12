#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For close() function
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string>

#define FAILED -1
#define SUCCESS 0

int main()
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	// Creating socket file descriptor
	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Set the socket to non-blocking mode
	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("fcntl");
		exit(EXIT_FAILURE);
	}
	// SOL_SOCKET   /* options for socket level */ Level number for (get/set)sockopt() to apply to socket itself.
	// SO_REUSEADDR /* allow local address reuse */
	// SO_REUSEPORT /* allow local address & port reuse */

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				   &opt, sizeof(opt)) == FAILED)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // localhost
	address.sin_port = htons(6667);

	// Binding socket to the port 6667
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) == FAILED)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	// Loop indefinitely to accept and handle connections
	printf("Waiting for connections...\n");
	while (1)
	{
		while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
		{
		}
		printf("Got connection...\n");
		// Handle the connection, for example, by sending a welcome message
		std::string message = "Hello from server\n";
		send(new_socket, message.c_str(), message.length(), 0);
		std::string message1;
		// close(new_socket);
	}
	// Close the server socket at the end (though in this case, we'll never get here)
	// close(server_fd);

	return 0;
}
