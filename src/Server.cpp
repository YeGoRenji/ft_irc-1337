#include <Replies.hpp>
#include <Server.hpp>
#include <Client.hpp>

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
			int newClientFd = newClient.getFd();
			// clients.push_back(newClient);
			clients[newClientFd] = newClient;
			// clients.insert(make_pair(, newClient));
			fds[0].revents = 0;
			fds.push_back((pollfd){ newClientFd, POLLIN, 0 });
		}


		for (size_t i = fds.size() - 1; i > 0; --i) {
			if (fds[i].revents & POLLHUP) {
				Client &currentCLient = clients[fds[i].fd];
				quitUser(currentCLient, fds, "Leaving...");
			}
			else if (fds[i].revents & POLLIN) {
				Client &currentCLient = clients[fds[i].fd];
				string data;
				currentCLient >> data;
				cout << "\nGot <" << data << "> from Client " << currentCLient.getFd() << " (" << currentCLient.getNick() << ") " << &currentCLient << endl;
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
		quitUser(currentCLient, fds, tokens.size() == 2 ? tokens[1] : "Leaving...");
	else if (tokens[0] == "JOIN") // TODO : DEFINITALLY STILL NOT FINISHED
		AddClientoChannel(currentCLient, tokens);
	else if (tokens[0] == "PART")
		handlePART(currentCLient, tokens); // TODO : STILL NOT FINISHED
	else if (tokens[0] == "KICK")
		KickClientFromChannel(currentCLient, tokens);
	else if (tokens[0] == "INVITE")
		InviteClientFromChannel(currentCLient, tokens);
	else if (tokens[0] == "TOPIC")
		TopicClientFromChannel(currentCLient, tokens);
	else
	{
		cerr << "lach msayft command khawi a wld l 9a7ba" << endl;
	}
}

bool Server::checkPassword(string input) {
	return this -> password == input;
}

void Server::quitUser(Client &currClient, vector<pollfd> &fds, string reason)
{
	vector<pollfd>::iterator it = fds.begin();

	while (it != fds.end() && it->fd != currClient.getFd())
		it++;

	Errors::CUSTOM_CLIENT_GONE_TO_EDGE(currClient);

	currClient.leaveAllChannels(*this, reason);
	currClient.disconnect();

	clients.erase(currClient.getFd());
	fds.erase(it);
}

bool Server::checkUserExistence(string nickName)
{
	return (getClientFromNick(nickName) != clients.end());
}

void Server::AddClientoChannel(Client &client, vector<string> &tokens)
{
	// TODO: Refactor this method so it looks like RemoveClientFromChannel
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
		if (it->name == "0")
		{
			client.leaveAllChannels(*this, "Left all channels...");
			continue ;
		}
		if (!Channel::isValidName(it->name))
		{
			Errors::ERR_NOSUCHCHANNEL(it->name, client, *this);
			continue;
		}
		// check if channel exists if not create it
		map<string, Channel>::iterator channelIt = getChannel(it -> name);
		if (channelIt == channels.end())
		{
//			cerr << "channel " << it -> name << " does not exist, creating it .." << endl;
			channelIt = createChannel(it -> name, it -> password);
			channelIt -> second.addOperator(client);
		}
		else
		{
//			cerr << "channel " << it -> name << " exists!" << endl;
			// channel has pass but user didn't supply it
			if (channelIt -> second.hasPassword() && (it -> password).empty())
			{
				Errors::ERR_BADCHANNELKEY(it -> name, client, *this);
				return;
			}
			// user supplied a wrong password
			if (!channelIt -> second.checkPassword(it -> password))
			{
				Errors::ERR_BADCHANNELKEY(it -> name, client, *this);
				return;
			}
			// do nothing, currChannel already points to the right channel
			// now just add the user to the channel and broadcast
		}
		// adduser to channel
		channelIt -> second.addMember(client);
		if (!channelIt->second.getTopic().empty())
			Replies::RPL_TOPIC(channelIt->second, client, *this);
		channelIt -> second.sendClientsList(channelIt->second, client, *this);
		// channelIt -> second.broadcastAction(client, JOIN);
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

map<int, Client>::iterator Server::getClientFromNick(string &nick)
{
	map<int, Client>::iterator it = clients.begin();

	for(; it != clients.end(); it++)
	{
		if (it -> second.getNick() == nick)
		{
			return (it);
		}
	}
	return it;
}

void Server::handlePART(Client &client, vector<string> &tokens)
{
	size_t tokens_len = tokens.size();
	string command = tokens[0];
	if (tokens_len == 1)
		return Errors::ERR_NEEDMOREPARAMS(command, client, *this);

	string reason = "Client gone to edge";

	if (tokens_len == 3) {
		reason = tokens.back();
	}

	vector<channelInfo> ch;
	parseChannelCommand(ch, tokens[1], "");

	string channelName;
	for(size_t i = 0; i < ch.size(); i++)
	{
		channelName = ch[i].name;
		map<string, Channel>::iterator ch = getChannel(channelName);
		if (ch == channels.end())
			 return Errors::ERR_NOSUCHCHANNEL(channelName, client, *this);

		string clientNick = client.getNick();
		if (!ch -> second.hasMember(clientNick))
	 		return Errors::ERR_NOTONCHANNEL(channelName, client, *this);

		// TODO: something is missing in the message sent here
		// ch -> second.broadcastAction(client, PART);
		cerr << "Removing " << client.getNick() << " from channel " << channelName << ", Reason: " << reason << endl;
		RemoveMemberFromChannel(ch->second, client, reason);
	}
}

void Server::KickClientFromChannel(Client &client, vector<string> &tokens)
{
	size_t tokens_len = tokens.size();
	string &command = tokens[0];

	if (tokens_len < 3)
		return Errors::ERR_NEEDMOREPARAMS(command, client, *this);

	string &channel = tokens[1];

	if (channel[0] != '#') // NEED to check the return error 
		return Errors::ERR_NOSUCHCHANNEL(channel, client, *this);

	map<string, Channel>::iterator chIt = getChannel(channel);

	if (chIt == channels.end())
		return Errors::ERR_NOSUCHCHANNEL(channel, client, *this);

	Channel &channelObj = chIt->second;

	if (!channelObj.hasMember(client.getNick()))
		return Errors::ERR_NOTONCHANNEL(channel, client, *this);

	if (!channelObj.isOperator(client.getNick()))
		return Errors::ERR_CHANOPRIVSNEEDED(channel, client, *this);
	
	string &kickedNick = tokens[2];

	if (!channelObj.hasMember(kickedNick))
		return Errors::ERR_USERNOTINCHANNEL(kickedNick, channel, client, *this);

	channelObj.removeMember(kickedNick);
	// TODO: KICK THE USER

	/*
		send the message to ALL and then erase the user from the channel 
	*/
}

/* 
 			todo above 
void Channel::invite(Client* client) {
    _invited.push_back(client);
}

*/

void Server::InviteClientFromChannel(Client &client, vector<string> &tokens)
{
	size_t tokens_len = tokens.size();
	string &command = tokens[0];

	if (tokens_len < 3)
		return Errors::ERR_NEEDMOREPARAMS(command, client, *this);

	string &channel = tokens[2];

	map<string, Channel>::iterator chIt = getChannel(channel);

	if (chIt == channels.end())
		return Errors::ERR_NOSUCHCHANNEL(channel, client, *this);

	Channel &channelObj = chIt->second;

	if (!channelObj.hasMember(client.getNick()))
		return Errors::ERR_NOTONCHANNEL(channel, client, *this);

	if (!channelObj.isOperator(client.getNick()))
		return Errors::ERR_CHANOPRIVSNEEDED(channel, client, *this);

	string &invitedNick = tokens[1];

	if (channelObj.hasMember(invitedNick))
		return Errors::ERR_USERONCHANNEL(invitedNick, channel, client, *this);

	
	
	Replies::RPL_INVITING(invitedNick, channel, client, *this);
	Client &invited = getClientFromNick(invitedNick)->second;
	Replies::notifyInvite(client, invited, channel);
	// Replies::notifyInvite(Client &inviter, Client &invited, string &channelName);
	// TODO: INVITE THE USER
	/*
		// channelObj.addInvitedUser()
		Add user to idk attribute about invitedusers or somthing !!
		search for "todo above" 		
	*/
	
}

void Server::TopicClientFromChannel(Client &client, vector<string> &tokens)
{
	size_t tokens_len = tokens.size(); // TOPIC #channel :new_topic
	string &command = tokens[0];

	if (tokens_len < 2)
		return Errors::ERR_NEEDMOREPARAMS(command, client, *this);

	string &channel = tokens[1];

	map<string, Channel>::iterator chIt = getChannel(channel);

	if (chIt == channels.end())
		return Errors::ERR_NOSUCHCHANNEL(channel, client, *this);

	Channel &channelObj = chIt->second;
	
	if (!channelObj.hasMember(client.getNick()))
		return Errors::ERR_NOTONCHANNEL(channel, client, *this);

	if (!channelObj.isOperator(client.getNick()))
		return Errors::ERR_CHANOPRIVSNEEDED(channel, client, *this);
	
	

	if (tokens_len == 2)
	{
		if (channelObj.getTopic().empty())
			Replies::RPL_NOTOPIC(channel, client, *this);
		else
			Replies::RPL_TOPIC(channel, channelObj.getTopic(), client, *this);
	}
	else
	{
		string &newTopic = tokens[2];

		channelObj.setTopic(newTopic, client.getNick());
		Replies::RPL_TOPIC(channel, newTopic, client, *this);
		Replies::RPL_TOPICWHOTIME(channel, channelObj.getTopicSetter(), channelObj.getTopicSetTime(), client, *this);
	}
  
}

void Server::RemoveMemberFromChannel(Channel &channel, Client &client, string reason)
{
	channel.removeMember(client, reason);
	if (channel.getMemberCount() == 0)
		channels.erase(channel.getChannelName());
}

map<string, Channel> &Server::getChannels() {
	return (this -> channels);
}
