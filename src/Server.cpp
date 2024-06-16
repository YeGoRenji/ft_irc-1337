/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 21:39:27 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/16 21:55:19 by afatimi          ###   ########.fr       */
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

		for (size_t i = fds.size() - 1; i > 0; --i) {
			if (fds[i].revents & POLLHUP) {
				Client &currentCLient = clients[i - 1];
				cout << "Client " << fds[i].fd << " disconnected" << endl;
				quitUser(currentCLient, fds); // TODO : send an error
			}
			else if (fds[i].revents & POLLIN) {
				Client &currentCLient = clients[i - 1];
				string data;

				currentCLient.getFdObject() >> data;
				cout << "Got <" << data << ">" << endl;
				vector<string> tokens = Utility::getCommandTokens(data);
				fds[i].revents = 0;
				if (tokens.size())
					commandsLoop(currentCLient, tokens, fds);
			}
		}
	}
}

void Server::commandsLoop(Client &currentCLient, vector<string> &tokens, vector<pollfd> &fds)
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
		quitUser(currentCLient, fds);
	else if (tokens[0] == "JOIN")
		AddClientoChannel(currentCLient, tokens);
}

bool Server::checkPassword(string input) {
	return this -> password == input;
}

void Server::quitUser(Client &client, vector<pollfd> &fds)
{
	size_t clients_num = clients.size();
	size_t i = 0;

	while(i < clients_num && (&client != &clients[i]));

	Errors::CUSTOM_CLIENT_GONE_TO_EDGE(client);

	clients[i].disconnect();
	clients.erase(clients.begin() + i);
	fds.erase(fds.begin() + i);
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

void Server::AddClientoChannel(Client &client, vector<string> tokens)
{
	string Channels;
	string Passwords;

	cout << "tokens size : " << tokens.size() << endl;

	if (tokens.size() == 1)
	{
		// the next lines only for debugging purposes
		map<string, Channel>::iterator it = channels.begin();
		for(; it != channels.end(); it++)
			cout << "channel name : " << (it -> second).getChannelName() << endl;

		Errors::ERR_NEEDMOREPARAMS(tokens[0], client, *this);
		return;
	}

	cout << "- there are " << channels.size() << " channels in the server" << endl;

	map<string, Channel>::iterator it = channels.begin();
	for(; it != channels.end(); it++)
		cout << "channel name : " << (it -> second).getChannelName() << endl;

	// TODO : WACH MAKAYNACH CHI WAY TO CHECK THE EXISTENCE OF CHI ELEMENT FL MAP WIRTH O(1) COMPLEXITY???
	// imma use count cause I don't wanna use exceptions
	cout << "channels map size : " << channels.size() << endl;
	if (tokens.size() == 2) //  channels without passwords in the form 'JOIN #chan[,chan2,chan3..]'
	{
		if (!ChannelAlreadyExists(Channels))
		{
			cout << "Channel " << tokens[1] << " does not exist" << endl;
			Channel ch(tokens[1], "");
			ch.addMember(client);
			channels.insert(std::pair<string,Channel>(tokens[1], Channel(tokens[1], "")));
		}
		else
		{
			cout << "Channel " << tokens[1] << " exists" << endl;
			Channel ch = channels[tokens[1]];
			ch.addMember(client);
		}
		// TODO : check if channel exists
		cout << "channels map size : " << channels.size() << endl;
		cout << "channels : " << tokens[1] << endl;
		cout << "passwords: " << "None" << endl;
	}
	else // channels with passwords in the form 'JOIN #chan[,chan2,chan3..] pass[,pass2,pass3..]'
		 // TODO : handle later
	{
		cout << "channels : " << tokens[1] << endl;
		cout << "passwords: " << tokens[2] << endl;
	}
}

bool Server::ChannelAlreadyExists(string name)
{
	return (channels.count(name));
}
