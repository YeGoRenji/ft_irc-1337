/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 21:39:27 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/06 11:51:55 by afatimi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include <cstdio>
#include <cstring>
#include <sys/socket.h>

int chk(int status, const std::string msg) {
	// TODO: use throw ?
	if (status < 0) {
		std::cerr << "Error[" << status << "]\t" << msg << ", Reason: " << strerror(errno) << endl;
		exit(status);
	}
	return status;
}

Server::Server(int port, string pass): password(pass)
{
	std::cout << "Server: Parameter constructor called" << endl;

	serverSocket.setValue(chk(socket(AF_INET, SOCK_STREAM, 0), "Couldn't open socket"));

	cout << "Server socket : " << serverSocket.getValue() << endl;

	int yes = 1;

	chk(setsockopt(serverSocket.getValue(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)), "Couldn't set socket option");

	sockaddr_in address;
	bzero(&address, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_port   = htons(port);
	address.sin_addr.s_addr = 0;

	chk(bind(serverSocket.getValue(), (sockaddr *)&address, sizeof(address)), "Couldn't bind socket to address");

	chk(listen(serverSocket.getValue(), 10), "Couldn't Listen to socket");
}

Server::~Server()
{
	std::cout << "Server: Destructor called" << endl;
}

void Server::start() {

	while(69) {
		pollfd fds[1 + clients.size()];

		fds[0] = (pollfd){ serverSocket.getValue(), POLLIN, 0 };
		for (size_t i = 0; i < clients.size(); ++i) {
			fds[i + 1] = (pollfd){ clients[i].getFd(), POLLIN, 0 };
		}

		chk(poll(fds, 1 + clients.size(), WAIT_ANY), "poll failed");

		if (fds[0].revents & POLLIN) {
			Client newClient = chk(accept(serverSocket.getValue(), NULL, NULL), "Couldn't accept connection");
			if (newClient.login(*this))
			{
				clients.push_back(newClient);
				cout << "New client connected " << newClient.getFd() << ", size = " << clients.size() << endl;
				fds[0].revents = 0;
			}
		}

		for (size_t i = 1; i < 1 + clients.size(); ++i) {
			if (fds[i].revents & POLLHUP) {
				cout << "Client " << fds[i].fd << " disconnected" << endl;
				clients[i - 1].disconnect();
				clients.erase(clients.begin() + i - 1);
				fds[i].revents = 0;
			}
			if (fds[i].revents & POLLIN) {
				string data;
				clients[i - 1].getFdObject() >> data;
				cout << "Got <" << data << ">" << endl;
				fds[i].revents = 0;
			}
		}
	}
}

bool Server::checkPassword(string input) {
	return this -> password == input;
}

bool Server::checkUserExistence(string NickName)
{
	vector<Client>::iterator it;
	cout << "conncted Nicknames list : " << endl;
	for(it = clients.begin(); it != clients.end(); it++)
	{
		cout << "<" << it -> getNick() << ">" << endl;
		if (it -> getNick() == NickName)
		{
			cout << "NickName found!!" << endl;
			return true;
		}
	}
	cout << "NickName wasn't found --, carry on connecting mate" << endl;
	return (false);
}
