#include <Server.hpp>

string Server::serverName = "IRatherComeServer.mybasement";
void	replyModeNotify(Client &client, Channel &channel, string modeString, string param, Server &server);

string	toStr(long nbr) {
    std::string        ret;
    std::ostringstream convert;
    convert << nbr;
    ret = convert.str();
    return ret;
}

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
	signal(SIGPIPE, SIG_IGN);

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
			// TODO: maybe we shouldn't call check when accept fails (We should just ignore client ?)
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

				bool clientSentCTRL_C = currentCLient >> data;

				if (clientSentCTRL_C)
				{
					quitUser(currentCLient, fds, "Client *spanked* CTRL-C");
					continue;
				}
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

//	cerr << "token size : " << tokens[0].size() << endl;

	if (tokens[0] == "PASS")
		currentCLient.passHandler(*this, tokens);
	else if (tokens[0] == "NICK")
		currentCLient.handleNICK(*this, tokens);
	else if (tokens[0] == "USER")
		currentCLient.setUsernameAndRealName(*this, tokens);
	else if (tokens[0] == "QUIT")
		quitUser(currentCLient, fds, tokens.size() == 2 ? tokens[1] : "Leaving...");
	else if (tokens[0] == "JOIN") // TODO : DEFINITALLY STILL NOT FINISHED
		handleJOIN(currentCLient, tokens);
	else if (tokens[0] == "PART")
		handlePART(currentCLient, tokens); // TODO : STILL NOT FINISHED
	else if (tokens[0] == "KICK")
		handleKICK(currentCLient, tokens);
	else if (tokens[0] == "INVITE")
		InviteClientFromChannel(currentCLient, tokens);
	else if (tokens[0] == "TOPIC")
		TopicClientFromChannel(currentCLient, tokens);
	else if (tokens[0] == "PRIVMSG")
		handlePRIVMSG(currentCLient, tokens);
	else if (tokens[0] == "MODE")
		ModeClientFromChannel(currentCLient, tokens);
	else
		cerr << "ach had l command dzb???" << endl;
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

void Server::handleJOIN(Client &client, vector<string> &tokens)
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

			if (!channelIt->second.canBeJoinedBy(client, it->password, *this))
				return;

			// do nothing, currChannel already points to the right channel
			// now just add the user to the channel and broadcast
		}
		Channel &channelObj = channelIt->second;
		// adduser to channel
		channelObj.addMemberAndBroadcast(client);
		if (!channelObj.getTopic().empty())
			Replies::RPL_TOPIC(channelObj.getChannelName(), channelObj.getTopic(), client, *this);
		channelObj.sendClientsList(client, *this);
		// channelIt -> second.broadcastAction(client, JOIN);
	}
}

// TODO: Change methode name
void Server::parseChannelCommand(vector<channelInfo> &ch, string channelsTokens, string passwordsTokens)
{
	size_t i;
	// TODO : refuse channels with space in their names
	vector<string> channelNames = Utility::splitTokensByChar(channelsTokens, ',');
	vector<string> passwords = Utility::splitTokensByChar(passwordsTokens, ',');

	cerr << "channels size = " << channelNames.size() << endl;
	cerr << "passwords size = " << passwords.size() << endl;

	for (i = 0; i < channelNames.size(); i++)
		ch.push_back((channelInfo){.name=channelNames[i], .password=""});

	size_t min_iter = min(channelNames.size(), passwords.size());
	for (i = 0; i < min_iter; i++)
		ch[i].password = passwords[i];
}

map<string, Channel>::iterator Server::createChannel(string name, string password)
{
    pair<map<string, Channel>::iterator, bool> result = channels.insert(make_pair(name, Channel(name, password, CHANNEL_MODES::NO_MODE)));
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
			return (it);
	}
	return clients.end();
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

void Server::handleKICK(Client &client, vector<string> &tokens)
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

	vector<channelInfo> kickedUsers;

	parseChannelCommand(kickedUsers, tokens[2], "");

	string reason = "Kicked by " + client.getNick();

	if (tokens_len == 4)
		reason = tokens.back();

	for (size_t i = 0; i < kickedUsers.size(); i++)
	{
		if (!channelObj.hasMember(client.getNick()))
			return Errors::ERR_NOTONCHANNEL(channel, client, *this);
		if (!channelObj.hasMember(kickedUsers[i].name))
		{
			Errors::ERR_USERNOTINCHANNEL(kickedUsers[i].name, channel, client, *this);
			continue;
		}
		Client &memberTokick = *channelObj.getMembers()[kickedUsers[i].name];

		Replies::notifyKick(client, memberTokick, channel);
		RemoveMemberFromChannel(channelObj, memberTokick, reason);
	}

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
	channelObj.invite(&invited);
	Replies::notifyInvite(client, invited, channel);

	// TODO: INVITE THE USER
	/*
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
		else {
			Replies::RPL_TOPIC(channel, channelObj.getTopic(), client, *this);
			Replies::RPL_TOPICWHOTIME(channel, channelObj.getTopicSetter(), channelObj.getTopicSetTime(), client, *this);
		}
	}
	else
	{
		string &newTopic = tokens[2];

		channelObj.setTopic(newTopic, client.getNick());
		Replies::RPL_TOPIC(channel, newTopic, client, *this);
		// TODO: broadcast new topic to all members of channel
	}
}

void Server::RemoveMemberFromChannel(Channel &channel, Client &client, string reason)
{
	channel.removeMemberAndBroadcast(client, reason);
	if (channel.getMemberCount() == 0)
		channels.erase(channel.getChannelName());
}

map<string, Channel> &Server::getChannels() {
	return (this -> channels);
}

vector<Channel *> Server::getChannelsWithMember(string nick) {
	map<string, Channel>::iterator channelsIt = channels.begin();
	map<string, Channel>::iterator channelsIte = channels.end();

	vector<Channel *> result;

	for (; channelsIt != channelsIte; ++channelsIt)
		if (channelsIt->second.hasMember(nick))
			result.push_back(&channelsIt->second);
	return result;
}

void Server::handlePRIVMSG(Client &sender, vector<string> &tokens)
{
	if (tokens.size() == 1)
		return Errors::ERR_NORECIPIENT(tokens[0], sender, *this);

	else if (tokens.size() == 2)
		return Errors::ERR_NOTEXTTOSEND(sender, *this);

	// targets could either be [a] channel[s] or user[s]
	string &targetsTokens = tokens[1];
	string &message = tokens[2];

	vector<string> targets = Utility::splitTokensByChar(targetsTokens, ',');

	vector<string>::iterator targetNamesIt = targets.begin();
	vector<string>::iterator targetNamesIte = targets.end();
	for(; targetNamesIt != targetNamesIte; targetNamesIt++)
	{
		string targetName = *targetNamesIt;
		bool chanOpsOnly = false;
		// check for a first special charater and erase it if exists
		while (targetName[0] == '%' || targetName[0] == '@')
		{
			chanOpsOnly = true;
			targetName.erase(0, 1);
			cout << "targetName <" << targetName << ">" << endl;
		}

		if (this -> hasChannel(targetName))
		{
			Channel &ch = getChannel(targetName) -> second;

			if (!ch.hasMember(sender.getNick()))
				return Errors::ERR_CANNOTSENDTOCHAN(ch.getChannelName(), sender, *this);

			map<string, Client*> *targetMembersGroup;
			if (chanOpsOnly)
				targetMembersGroup = &(ch.getChanOps());
			else
				targetMembersGroup = &(ch.getMembers());

			// TODO : refractor this garbage
			string reply = ":";
			reply += sender.getNick();
			reply += "!";
			reply += sender.getUsername();
			reply += "@";
			reply += sender.getIp();
			reply += " PRIVMSG ";
			reply += ch.getChannelName();
			reply += " :";
			reply += message;

			ch.broadcastMessageToGroup(reply, *targetMembersGroup, sender.getNick());
			ch.addMessage(sender.getNick(), message);
		}
		else if (this -> hasMember(targetName))
		{
			map<int, Client>::iterator receiverIt = getClientFromNick(targetName);
			if (receiverIt -> second.getNick() == sender.getNick())
				continue;

			Replies::RPL_PRIVMSG(sender, receiverIt -> second, message);
		}
		else
			return Errors::ERR_NOSUCHNICK(targetName, sender, *this);
	}
}

bool Server::hasMember(string &nick) {
	return (getClientFromNick(nick) != clients.end());
}

bool Server::hasChannel(string ChannelName)
{
	return (getChannel(ChannelName) != channels.end());
}
void	error(bool state, char c, vector<string>& token, Channel& channel, Client* client, Server &server)
{
	(void)state;
    (void)channel;
    (void)client;

	Errors::ERR_UNKNOWNMODE(token[1], string(1, c), *client, server);
}

// handleInvite
void	handleInvite(bool state, char c, vector<string> &token, Channel &channel, Client *client, Server &server)
{
    (void)c;
    (void)token;
	(void)server;

	if (!state && channel.modeIsSet(CHANNEL_MODES::SET_INVITE_ONLY))
	{
		channel.unsetMode(CHANNEL_MODES::SET_INVITE_ONLY);
		replyModeNotify(*client, channel, "-i", "here", server);
	}
	else if (state && !channel.modeIsSet(CHANNEL_MODES::SET_INVITE_ONLY))
	{
		channel.setMode(CHANNEL_MODES::SET_INVITE_ONLY);
		replyModeNotify(*client, channel, "+i", "here", server);
	}
}
void	handleTopic(bool state, char c, vector<string> &token, Channel &channel, Client *client, Server &server)
{
    (void)c;
    (void)token;
	(void)server;

	if (!state && channel.modeIsSet(CHANNEL_MODES::SET_TOPIC))
	{
		channel.unsetMode(CHANNEL_MODES::SET_TOPIC);
		replyModeNotify(*client, channel, "-t", "here", server);
	}
	else if (state && !channel.modeIsSet(CHANNEL_MODES::SET_TOPIC))
	{
		channel.setMode(CHANNEL_MODES::SET_TOPIC);
		replyModeNotify(*client, channel, "+t", "here", server);
	}
}


void	replyModeNotify(Client &client, Channel &channel, string modeString, string param, Server &server)
{
	string message = ":";
	message += client.getNick();
	message += "!";
	message += client.getUsername();
	message += "@";
	message += client.getIp();
	message += " MODE ";
	message += channel.getChannelName();
	message += " ";
	message += modeString;
	message += " ";
	message += param;
	message += " ";

	Client &clien = server.getClientFromNick(client.getNick())->second;
	clien << message;
}


void	handleLimit(bool state, char c, vector<string> &token, Channel &channel, Client *client, Server &server)
{
    (void)c;
	(void)server;

	if (state && token.size() < 4)
		return Errors::ERR_NEEDMOREPARAMS(token[0], *client, server);

	if (!state && channel.modeIsSet(CHANNEL_MODES::SET_LIMIT))
	{
		channel.unsetMode(CHANNEL_MODES::SET_LIMIT);
		replyModeNotify(*client, channel, "-l", toStr(channel.getLimit()), server);
	}
	else if (state && !channel.modeIsSet(CHANNEL_MODES::SET_LIMIT))
	{
		string str;
		if (token[3][0] != ':')
			str = token[3];
		else
			str = token[3].substr(1);
		std::stringstream ss(str);
		unsigned long     limit;
		ss >> limit;

		channel.setLimit(limit);
		channel.setMode(CHANNEL_MODES::SET_LIMIT);

		replyModeNotify(*client, channel, "+l", toStr(limit), server);
	}
}

void	handleKey(bool state, char c, vector<string> &token, Channel &channel, Client *client, Server &server)
{
    (void)c;
	(void)server;

	// MODE #D -k :password
	// 0     1  2    3
	if (token.size() < 4)
		return Errors::ERR_NEEDMOREPARAMS(token[0], *client, server);

	string pass;
	if (token[3][0] != ':')
		pass = token[3];
	else
		pass = token[3].substr(1);

	if (!state && channel.modeIsSet(CHANNEL_MODES::SET_KEY))
	{
		if (pass != channel.getPassword())
			return Errors::ERR_KEYALREADYSET(client->getNick(), channel.getChannelName(), *client, server); // 467 client #channel :Channel key already set
		channel.unsetMode(CHANNEL_MODES::SET_KEY);
		replyModeNotify(*client, channel, "-k", pass, server);
	}
	else if (state && !channel.modeIsSet(CHANNEL_MODES::SET_KEY))
	{
	
		channel.setPassword(pass);

		channel.setMode(CHANNEL_MODES::SET_KEY);

		replyModeNotify(*client, channel, "+k", pass, server);
	}
}

void	handleOperator(bool state, char c, vector<string> &token, Channel &channel, Client *client, Server &server)
{
    (void)c;

	// MODE #D -o :nickname
	// 0     1  2    3
	if (token.size() < 4)
		return Errors::ERR_NEEDMOREPARAMS(token[0], *client, server);

	string nickToOp;
	if (token[3][0] != ':')
		nickToOp = token[3];
	else
		nickToOp = token[3].substr(1);

	if (!server.hasMember(nickToOp))
		return Errors::ERR_NOSUCHNICK(nickToOp, *client, server);

	if (!channel.hasMember(nickToOp))
		return Errors::ERR_USERNOTINCHANNEL(nickToOp, channel.getChannelName(), *client, server);

	// Client	*clientToOp = &server.getClientFromNick(str)->second;
	Client	*clientToOp = channel.getMembers()[nickToOp];

	if (!state && channel.isOperator(clientToOp->getNick()))
	{
		channel.removeOperator(*clientToOp);

		replyModeNotify(*client, channel, "-o", nickToOp, server);
	}
	else if (state && !channel.isOperator(clientToOp->getNick()))
	{
		channel.addOperator(*clientToOp);

		replyModeNotify(*client, channel, "+o", nickToOp, server);
	}

}

void	HandleFlags(std::string& modeString, std::vector<std::string>& token, Channel& channel, Client* client, Server &server)
{
	bool state = true;

	if (modeString[0] == '+' || modeString[0] == '-')
	{
		state = (modeString[0] == '+');
		modeString.erase(0, 1);
	}
	else
		return Errors::ERR_UNKNOWNMODE(token[1], string(1, modeString[0]), *client, server);
		// return Errors::ERR_UNKNOWNMODE(channel, modeString[0], client, server);

	void (*f[])(bool state, char c, std::vector<std::string>& tmp, Channel& channel, Client* client, Server &server) = {
		&error,
		&handleInvite,
		&handleTopic,
		&handleLimit,
		&handleKey,
		&handleOperator
	};
	for (size_t i = 0; i < modeString.size() && !std::strchr("-+", modeString[i]); ++i)
	{
		int index = (modeString[i] == 'i') * 1 + (modeString[i] == 't') * 2 + (modeString[i] == 'l') * 3 + (modeString[i] == 'k') * 4 + (modeString[i] == 'o') * 5;
		f[index](state, modeString[i], token, channel, client, server);
	}
	modeString.erase(0, modeString.find_first_of("-+"));
}

void Server::ModeClientFromChannel(Client &client, vector<string> &tokens)
{
	size_t tokens_len = tokens.size();
	string &command = tokens[0];

	if (tokens_len < 2)
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

	if (tokens_len == 2) // mode #d
	{
		// TODO: refractor to a seperate func
		string modeString = "+"; // TODO : get the deafult mode of the channel
		// i need that if modes is sets, add the charecter to the modeString
		if (channelObj.modeIsSet(CHANNEL_MODES::SET_INVITE_ONLY))
			modeString += "i";
		if (channelObj.modeIsSet(CHANNEL_MODES::SET_TOPIC))
			modeString += "t";
		if (channelObj.modeIsSet(CHANNEL_MODES::SET_LIMIT))
			modeString += "l";
		if (channelObj.modeIsSet(CHANNEL_MODES::SET_KEY))
			modeString += "k";

		Replies::RPL_CHANNELMODEIS(channel, client, modeString, *this); // 324
		Replies::RPL_CREATIONTIME (channel, channelObj.getCreationTime(), client, *this); // 329
	}
	else // mode #d +lik
	{
		// TODO: refractor to a seperate func
		if (!channelObj.isOperator(client.getNick()))
			return Errors::ERR_CHANOPRIVSNEEDED(channel, client, *this);

		string modeString;
		if (tokens[2][0] == '+' || tokens[2][0] == '-')
			modeString = tokens[2];
		else
			return Errors::ERR_UNKNOWNMODE(tokens[2], string(1, modeString[0]), client, *this);

		while(!modeString.empty())
			HandleFlags(modeString, tokens, channelObj, &client, *this);
	}
}
