#include "Command.h"

void Command::handleOper(const Message &msg)
{
	std::shared_ptr<Client> client_ptr = msg.getClientPtr();
	std::string requested_nick = client_ptr->getNickname();
	std::string client_ip = client_ptr->getIpAddress();
	std::string client_host = client_ptr->getHostname();
	std::cout << "host = " << client_host << " ip = " << client_ip << std::endl;
	int fd = client_ptr->getFd();
	std::vector <t_opers> operator_file = server_->getOperatorsFile();
	std::vector<std::string> params = msg.getParameters();
	if (params.size() != 2)
	{
		server_->send_response(fd, ERR_NEEDMOREPARAMS(client_ptr->getClientPrefix(), msg.getCommand()));
		return;
	}
	std::string user_input_nick = params[0];
	std::string user_input_pass = params[1];	
	if (requested_nick != user_input_nick)
	{
		server_->send_response(fd, ERR_NOOPERHOST(server_->getServerHostname(), requested_nick));
		return;
	}
	else
	{
		std::vector<t_opers>::iterator it;
		for (it = operator_file.begin(); it != operator_file.end();it++)
		{
			if (it->nick == user_input_nick)
			{
				if (it->password == user_input_pass)
				{
					if (it->hostmask == client_host || it->hostmask == client_ip)
					{
						server_->send_response(fd, RPL_YOUREOPER(server_->getServerHostname(), requested_nick));
						//Needs to set the mode O for the user as a local operator
						server_->insertInOperators(std::pair(fd, client_ptr)); // Because the mode is not ready yet
						return;
					}
					else
					{
						server_->send_response(fd, ERR_NOOPERHOST(server_->getServerHostname(), requested_nick));
						return;
					}
				}
				else
				{
					server_->send_response(fd, ERR_INCORPASS(server_->getServerHostname(), requested_nick));
					return;
				}
			}
		}
		if (it == operator_file.end())
		{
			server_->send_response(fd, ERR_NOOPERHOST(server_->getServerHostname(), requested_nick));
			return;
		}
	}
}