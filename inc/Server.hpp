/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atoof <atoof@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 20:02:20 by atoof             #+#    #+#             */
/*   Updated: 2024/04/02 20:02:20 by atoof            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <vector> // std::vector for storing clients in the server class
# include <thread> // std::thread for running the server in a separate thread
# include <mutex> // std::mutex for synchronizing access to the server's clients vector
# include <algorithm> // std::find_if for checking if a client is already connected
# include <cstdlib> // std::exit for exiting the server when all clients disconnect
# include <unistd.h> // for sleeping between client checks on UNIX systems (Linux, Mac OS X)
# include <cstdlib> // for std::stoi
# include "Client.hpp" // include the Client class

class Server
{
	private:
		int port;
		std::string password;

	public:
		Server(int port, std::string password);
		~Server();
};

#endif
