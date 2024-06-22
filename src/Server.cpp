/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 21:39:27 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/22 20:50:30 by ylyoussf         ###   ########.fr       */
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
	fcntl(serverSocket.getValue(), F_SETFL, O_NONBLOCK);

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
			int newClientFd = newClient.getFd();
			fcntl(newClientFd, F_SETFL, O_NONBLOCK);
			// clients.push_back(newClient);
			clients.insert(make_pair(newClient.getNick(), newClient));
			cout << "New client connected " << newClientFd << ", size = " << clients.size() << endl;
			fds[0].revents = 0;
			fds.push_back((pollfd){ newClientFd, POLLIN, 0 });
		}

		for (size_t i = fds.size() - 1; i > 0; --i) {
			if (fds[i].revents & POLLHUP) {
				Client &currentCLient = getClientFromFd(fds[i].fd);
				cout << "Client " << fds[i].fd << " disconnected" << endl;
				quitUser(currentCLient, fds);
				break;
			}
			else if (fds[i].revents & POLLIN) {
				Client &currentCLient = getClientFromFd(fds[i].fd);
				string data;
				cout << "DATA COMING to client: " << currentCLient.getNick() << endl;
				currentCLient.getFdObject() >> data;
				cout << "Got <" << data << "> from Client " << currentCLient.getFd() << endl;
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

void Server::quitUser(Client &currClient, vector<pollfd> &fds)
{
	size_t fds_num = fds.size();
	size_t i = 1;

	while(i < fds_num && (fds[i].fd != currClient.getFd())) {
		cerr << "Comparing " << fds[i].fd << " to " << currClient.getFd() << endl;
		i++;
	}
	// cerr << "Compared after while " << fds[i].fd << " to " << currClient.getFd() << endl;

	Errors::CUSTOM_CLIENT_GONE_TO_EDGE(currClient);

	currClient.disconnect();
	clients.erase(currClient.getNick());
	cerr << "i = " << i << endl;
	fds.erase(fds.begin() + i);
}

bool Server::checkUserExistence(string nickName)
{
	return (clients.find(nickName) != clients.end());
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
		// check if channel exists if not create it
		map<string, Channel>::iterator currChannel = getChannel(it -> name);
		if (currChannel == channels.end())
		{
			// TODO : ask someone if we should create any non-existant channel that the user suplied
			// cause the RFC has an error called ERR_NOSUCHCHANNEL(403)
			cerr << "channel " << it -> name << " does not exist, creating it .." << endl;
			currChannel = createChannel(it -> name, it -> password);
		}
		else
		{
			cerr << "channel " << it -> name << " exists!" << endl;
			// channel has pass but user didn't supply it
			if (currChannel -> second.hasPassword() && (it -> password).empty())
			{
				Errors::ERR_BADCHANNELKEY(it -> name, client, *this);
				return;
			}
			// user supplied a wrong password
			if (!currChannel -> second.checkPassword(it -> password))
			{
				Errors::ERR_BADCHANNELKEY(it -> name, client, *this);
				return;
			}
			// do nothing, currChannel already points to the right channel
			// now just add the user to the channel and broadcast
		}
		cout << "gonna add the member now" << endl;
		// adduser to channel
		currChannel -> second.addMember(client);
		// broadcast it // TODO : mn l a7san that user should be broadcasted before adding the user to the channel!
	}
}

void Server::parseChannelCommand(vector<channelInfo> &ch, string channelsTokens, string passwordsTokens)
{
	size_t i;
	replace(channelsTokens.begin(), channelsTokens.end(), ',', ' ');
	// TODO : refuse channels with space in their names
	vector<string> channelNames = Utility::getCommandTokens(channelsTokens);
	vector<string> passwords = Utility::getCommandTokens(passwordsTokens);

	for (i = 0; i < channelNames.size(); i++)
		ch.push_back((channelInfo){.name=channelNames[i], .password=""});

	size_t min_iter = min(channelNames.size(), passwords.size());
	for (i = 0; i < min_iter; i++)
		ch[i].password = passwords[i];
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

Client &Server::getClientFromFd(int fd)
{
	map<string, Client>::iterator it = clients.begin();

	for(; it != clients.end(); it++)
	{
		if (it -> second.getFd() == fd)
		{
			return (it -> second);
		}
	}
	cerr << "\033[31;1;4mBUG:\033[0m did not find client with fd " << fd << endl;
	return (it -> second);
}
