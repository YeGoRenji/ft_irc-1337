/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 21:39:27 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/22 13:21:44 by ylyoussf         ###   ########.fr       */
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
//	cerr << "connected Nicknames list : " << endl;
	for(it = clients.begin(); it != clients.end(); it++)
	{
//		cerr << "<" << it -> getNick() << ">" << endl;
		if (it -> getNick() == NickName)
		{
//			cerr << "NickName found!!" << endl;
			return true;
		}
	}
//	cerr << "NickName wasn't found --, carry on connecting mate" << endl << endl;
	return (false);
}

void Server::AddClientoChannel(Client &client, vector<string> tokens)
{
	string channelsTokens;
	string passwordsTokens;

	vector<channelInfo> ch;

	if (tokens.size() == 1)
	{
		// the next lines only for debugging purposes
		map<string, Channel>::iterator it = channels.begin();
			cerr << "channel name : ";
		for(; it != channels.end(); it++)
			cerr << "channelName = " << (it -> second).getChannelName() << ", count = " <<  channels.count((it -> second).getChannelName());

		Errors::ERR_NEEDMOREPARAMS(tokens[0], client, *this);
		return;
	}

	channelsTokens = tokens[1];
	if (tokens.size() == 3)
		passwordsTokens = tokens[2];

	parseChannelCommand(ch, channelsTokens, passwordsTokens);

	vector<channelInfo>::iterator it = ch.begin();
	vector<channelInfo>::iterator ite = ch.end();
	for(; it != ite; it++)
	{
		cerr << "handling channel : " << it -> name << endl;
		if (it -> password.empty())
		{
			// check if channel exists if not create it
			map<string, Channel>::iterator currChannel = getChannel(it -> name);
			if (currChannel == channels.end())
			{
				cerr << "channel " << it -> name << " does not exist, creating it .." << endl;
				currChannel = createChannel(it -> name, it -> password);
			}
			else
				cerr << "channel " << it -> name << " exists!" << endl;
			// adduser to channel
			currChannel -> second.addMember(client);
			// broadcast it // TODO : mn l a7san that user should be broadcasted before adding the user to the channel!
		};
		// else not
			// if !channelexists
				// create one with password
				// if !password correct
					// err
				// adduser to channel
				// broadcast it
	}
	return;


	// TODO : WACH MAKAYNACH CHI WAY TO CHECK THE EXISTENCE OF CHI ELEMENT FL MAP WIRTH O(1) COMPLEXITY???
	// imma use count cause I don't wanna use exceptions
	cout << "channels map size : " << channels.size() << endl;
	/*
	if (tokens.size() == 2) //  channels without passwords in the form 'JOIN #chan[,chan2,chan3..]'
	{
		if (!channelAlreadyExists(Channels))
		{
			cout << " -> Channel " << tokens[1] << " does not exist" << endl;
			Channel ch(tokens[1], "");
			ch.addMember(client);
			channels.insert(std::pair<string,Channel>(tokens[1], ch));
		}
		else
		{
			cout << " -> Channel " << tokens[1] << " exists" << endl;
			Channel &ch = channels[tokens[1]];
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
	*/
}

void Server::parseChannelCommand(vector<channelInfo> &ch, string channelsTokens, string passwordsTokens)
{
	size_t i;
	replace(channelsTokens.begin(), channelsTokens.end(), ',', ' ');
	// TODO : refuse channels with space in their names
	vector<string> channelNames = Utility::getCommandTokens(channelsTokens);
	vector<string> passwords = Utility::getCommandTokens(passwordsTokens);

	for (i = 0; i < channelNames.size(); i++)
		ch.push_back((channelInfo){.name=channelNames[i]});

	size_t min_iter = min(channelNames.size(), passwords.size());
	cerr << "min iter = " << min_iter << endl;
	for (i = 0; i < min_iter; i++)
		ch[i].password = passwords[i];

	for (i = 0; i < channelNames.size(); i++)
	{
		cerr << "name = " << ch[i].name << endl;
		cerr << "pass = " << ch[i].password << endl << endl;
	}
}

map<string, Channel>::iterator Server::createChannel(string name, string password)
{
    pair<map<string, Channel>::iterator, bool> result = channels.insert(make_pair(name, Channel(name, password)));
    return result.first;
}


map<string, Channel>::iterator Server::getChannel(string name)
{
	return channels.find(name);
}
