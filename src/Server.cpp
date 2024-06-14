/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 21:39:27 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/14 16:32:52 by afatimi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

string Server::serverName = "IRatherComeServer.mybasement";

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

string &Server::getServerName()
{
	return this -> serverName;
}

void Server::start() {

	map<string, void (Client::*)(vector<string>)> commandHandlers;

	vector<pollfd> fds;
	fds.push_back((pollfd){ serverSocket.getValue(), POLLIN, 0 });

	while(69) {

		chk(poll(fds.data(), fds.size(), WAIT_ANY), "poll failed");

		if (fds[0].revents & POLLIN) {
			Client newClient = chk(accept(serverSocket.getValue(), NULL, NULL), "Couldn't accept connection");
			clients.push_back(newClient);
			cout << "New client connected " << newClient.getFd() << ", size = " << clients.size() << endl;
			fds[0].revents = 0;
			fds.push_back((pollfd){ newClient.getFd(), POLLIN, 0 });
		}

		for (size_t i = 1; i < fds.size(); ++i) {
			if (fds[i].revents & POLLHUP) {
				cout << "Client " << fds[i].fd << " disconnected" << endl;
				clients[i - 1].disconnect(); // TODO : send an error
				clients.erase(clients.begin() + i - 1);
				fds[i].revents = 0;
				fds.erase(fds.begin() + i);
				break;
			}
			if (fds[i].revents & POLLIN) {
				Client &currentCLient = clients[i - 1];
				string data;

				currentCLient.getFdObject() >> data;
				cout << "Got <" << data << ">" << endl;
				vector<string> tokens = Utility::getCommandTokens(data);
				if (tokens.size())
					commandsLoop(currentCLient, tokens);
				fds[i].revents = 0;
			}
		}
	}
}

void Server::commandsLoop(Client &currentCLient, vector<string> &tokens)
{
	cout << "Client" << currentCLient.getFd() << ": " << currentCLient.isPassGiven() << endl;

	if (!currentCLient.isPassGiven() && tokens[0] != "PASS")
		return Errors::ERR_CUSTOM_NOT_AUTHED(currentCLient, *this);

	if (tokens[0] == "PASS")
		currentCLient.passHandler(*this, tokens);
	else if (tokens[0] == "NICK")
		currentCLient.setNick(*this, tokens);
	else if (tokens[0] == "USER")
		currentCLient.setUsernameAndRealName(*this, tokens);
	else if (tokens[0] == "QUIT")
		quitUser(currentCLient);

	//else if (tokens[1] == "NICK")
}

bool Server::checkPassword(string input) {
	return this -> password == input;
}

void Server::quitUser(Client &client)
{
	size_t clients_num = clients.size();
	size_t i = 0;

	while(i < clients_num && (&client != &clients[i]));

	cout << "before num of clients == " << clients.size() << endl;
	clients.erase(clients.begin() + i);
	cout << "after num of clients == " << clients.size() << endl;
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
