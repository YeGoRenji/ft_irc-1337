#include <Server.hpp>

string Server::serverName = "IRatherCumServer.myBasement";

int Server::NICKLEN = 30;
int Server::TOPICLEN = 369;
int Server::CHANNELLEN = 64;
int Server::MAXCHANNELS = 69;
int Server::MAXCLIENTS = 73; // from mbousouf <3

void	replyModeNotify(Client &client, Channel &channel, string modeString, string param, Server &server);


int chk(int status, const std::string msg, bool throwOnErr = true) {
	if (throwOnErr && status < 0) {
		stringstream ss;
		ss << "Error[" << status << "]\t" << msg << ", Reason: " << strerror(errno);
		throw std::runtime_error(ss.str());
	}
	return status;
}

Server::Server(uint16_t port, string pass): password(pass), creationTime(time(NULL)), channelsCount(0), clientsCount(0)
{
	signal(SIGPIPE, SIG_IGN);

	serverSocket.setValue(chk(socket(AF_INET, SOCK_STREAM, 0), "Couldn't open socket"));


	int yes = 1;

	chk(setsockopt(serverSocket.getValue(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)), "Couldn't set socket option");

	sockaddr_in address;
	bzero(&address, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_port   = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	chk(bind(serverSocket.getValue(), (sockaddr *)&address, sizeof(address)), "Couldn't bind socket to address");
	chk(listen(serverSocket.getValue(), SOMAXCONN), "Couldn't Listen to socket");

	cout << "Listening for connections..." << endl;
}

Server::~Server() {}

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
			fds[0].revents = 0;
			Client newClient = chk(accept(serverSocket.getValue(), NULL, NULL), "Couldn't accept connection", false);
			int newClientFd = newClient.getFd();

			if (newClientFd < 0)
				continue;

			if (clientsCount >= Server::MAXCLIENTS) {
				Errors::CUSTOM_SERVER_FULL(newClient);
				newClient.disconnect();
				continue ;
			}

			clients[newClientFd] = newClient;
			fds.push_back((pollfd){ newClientFd, POLLIN, 0 });
			clientsCount++;
		}

		for (size_t i = fds.size() - 1; i > 0; --i) {
			if (fds[i].revents & POLLHUP) {
				fds[i].revents = 0;
				Client &currentCLient = clients[fds[i].fd];
				quitUser(currentCLient, fds, "Leaving...");
			}
			else if (fds[i].revents & POLLIN) {
				fds[i].revents = 0;
				Client &currentCLient = clients[fds[i].fd];
				string data;

				currentCLient >> data;

				if (data.size() > 512) {
					Errors::ERR_INPUTTOOLONG(currentCLient, *this);
					continue;
				}
				vector<string> tokens = Utility::getCommandTokens(data);
				if (tokens.size())
					commandsLoop(currentCLient, tokens, fds);
			}
		}
	}
}

void Server::commandsLoop(Client &currentCLient, vector<string> &tokens, vector<pollfd> &fds)
{
	string command = Utility::toUpper(tokens[0]);

	if (!currentCLient.isAuthenticated() && command != "PASS" && command != "NICK" && command != "USER" && command != "QUIT")
		return Errors::ERR_CUSTOM_NOT_AUTHED(currentCLient, *this);

	if (command == "PASS")
		currentCLient.passHandler(*this, tokens);
	else if (command == "NICK")
		currentCLient.handleNICK(*this, tokens);
	else if (command == "USER")
		currentCLient.setUsernameAndRealName(*this, tokens);
	else if (command == "QUIT")
		quitUser(currentCLient, fds, tokens.size() == 2 ? tokens[1] : "Leaving...");
	else if (command == "JOIN")
		handleJOIN(currentCLient, tokens);
	else if (command == "PART")
		handlePART(currentCLient, tokens);
	else if (command == "KICK")
		handleKICK(currentCLient, tokens);
	else if (command == "INVITE")
		handleINVITE(currentCLient, tokens);
	else if (command == "TOPIC")
		handleTOPIC(currentCLient, tokens);
	else if (command == "PRIVMSG")
		handlePRIVMSG(currentCLient, tokens);
	else if (command == "MODE")
		handleMODE(currentCLient, tokens);
	else
		Errors::ERR_UNKNOWNCOMMAND(command, currentCLient, *this);
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
	clientsCount--;
	fds.erase(it);
}

bool Server::checkUserExistence(string nickName)
{
	return (getClientFromNick(nickName) != clients.end());
}

Channel &Server::getChannelOrCreate(Client &client, channelInfo &chInfo)
{
	map<string, Channel>::iterator channelIt = getChannel(chInfo.name);

	if (channelIt == channels.end()) // Channel doesn't exist
	{
		if (Server::channelsCount >= Server::MAXCHANNELS)
			throw TooManyChannels();
		channelIt = createChannel(chInfo.name, chInfo.password);
		Server::channelsCount++;
		channelIt -> second.addOperator(client);
	}
	else // Channel Exists
	{
		if (!channelIt->second.canBeJoinedBy(client, chInfo.password, *this))
			throw CannotJoin();
	}

	return channelIt->second;
}

void Server::handleJOIN(Client &client, vector<string> &tokens)
{
	if (tokens.size() <= 1)
		return Errors::ERR_NEEDMOREPARAMS(tokens[0], client, *this);

	string channelsTokens;
	string passwordsTokens;

	channelsTokens = tokens[1];
	if (tokens.size() >= 3)
		passwordsTokens = tokens[2];

	vector<channelInfo> ch;
	parseChannelsToken(ch, channelsTokens, passwordsTokens);

	vector<channelInfo>::iterator it = ch.begin();
	vector<channelInfo>::iterator ite = ch.end();

	channelInfo chToJoin;
	for(; it != ite; it++)
	{
		chToJoin = *it;
		if (chToJoin.name == "0")
		{
			client.leaveAllChannels(*this, "Left all channels...");
			continue ;
		}
		if (!Channel::isValidName(chToJoin.name))
		{
			Errors::ERR_NOSUCHCHANNEL(chToJoin.name, client, *this);
			continue;
		}

		chToJoin.name = Utility::toLower(chToJoin.name);
		try {
			Channel &channelObj = getChannelOrCreate(client, chToJoin);

			channelObj.addMemberAndBroadcast(client);
			channelObj.sendTopic(client, *this);
			channelObj.sendClientsList(client, *this);
		} catch (TooManyChannels &e) {
			Errors::CUSTOM_TOO_MANY_CHANNELS(client);
		} catch (exception &e) {}
	}
}

void Server::parseChannelsToken(vector<channelInfo> &ch, string channelsTokens, string passwordsTokens)
{
	size_t i;
	vector<string> channelNames = Utility::splitTokensByChar(channelsTokens, ',');
	vector<string> passwords = Utility::splitTokensByChar(passwordsTokens, ',');

	for (i = 0; i < channelNames.size(); i++)
		ch.push_back((channelInfo){.name=channelNames[i].substr(0, Server::CHANNELLEN), .password=""});

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
	return channels.find(Utility::toLower(name));
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
	if (tokens_len <= 1)
		return Errors::ERR_NEEDMOREPARAMS(command, client, *this);

	string reason = "Client gone to edge";

	if (tokens_len >= 3)
		reason = tokens[2];

	vector<channelInfo> ch;
	parseChannelsToken(ch, tokens[1], "");

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

	if (channel[0] != '#')
		return Errors::ERR_NOSUCHCHANNEL(channel, client, *this);

	map<string, Channel>::iterator chIt = getChannel(channel);

	if (chIt == channels.end())
		return Errors::ERR_NOSUCHCHANNEL(channel, client, *this);

	Channel &channelObj = chIt->second;

	if (!channelObj.hasMember(client.getNick()))
		return Errors::ERR_NOTONCHANNEL(channel, client, *this);

	if (!channelObj.isOperator(client.getNick()))
		return Errors::ERR_CHANOPRIVSNEEDED(channel, client, *this);

	vector<string> kickedNicks = Utility::splitTokensByChar(tokens[2], ',');

	string reason = "Kicked by " + client.getNick();

	if (tokens_len >= 4)
		reason = tokens.back();

	for (size_t i = 0; i < kickedNicks.size(); i++)
	{
		if (!channelObj.hasMember(client.getNick()))
			return Errors::ERR_NOTONCHANNEL(channel, client, *this);
		if (!channelObj.hasMember(kickedNicks[i]))
		{
			Errors::ERR_USERNOTINCHANNEL(kickedNicks[i], channel, client, *this);
			continue;
		}
		Client &memberTokick = *channelObj.getMembers()[kickedNicks[i]];

		Replies::notifyKick(client, memberTokick, channel);
		RemoveMemberFromChannel(channelObj, memberTokick, reason);
	}

}

void Server::handleINVITE(Client &client, vector<string> &tokens)
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
}

void Server::handleTOPIC(Client &client, vector<string> &tokens)
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
		if (channelObj.modeIsSet(CHANNEL_MODES::SET_TOPIC) && !channelObj.isOperator(client.getNick()))
			return Errors::ERR_CHANOPRIVSNEEDED(channel, client, *this);

		string newTopic = tokens[2].substr(0, Server::TOPICLEN);

		channelObj.setTopic(newTopic, client.getNick());
		Replies::RPL_TOPIC(channel, newTopic, client, *this);
		Replies::RPL_TOPICWHOTIME(channel, channelObj.getTopicSetter(), channelObj.getTopicSetTime(), client, *this);
		channelObj.broadcastMessageToGroup(":" + client.getNick() + "!" + client.getUsername() + "@" + client.getIp() + " TOPIC " + channel + " :" + newTopic, channelObj.getMembers(), client.getNick());
	}
}

void Server::RemoveMemberFromChannel(Channel &channel, Client &client, string reason)
{
	channel.removeMemberAndBroadcast(client, reason);
	if (channel.getMemberCount() == 0)
	{
		channels.erase(channel.getChannelName());
		Server::channelsCount--;
	}
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

			string reply = sender.craftSourceMessage("PRIVMSG", ch.getChannelName() + " :" + message);

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
void	error(bool state, char c, vector<string> &token, Channel &channel, Client &client, Server &server, std::vector<std::string> &Args)
{
	(void)state;
    (void)channel;
	(void)Args;
	(void)token;

	Errors::ERR_UNKNOWNMODE(string(1, c), client, server);
}

// handleInvite
void	handleInvite(bool state, char c, vector<string> &token, Channel &channel, Client &client, Server &server, std::vector<std::string> &Args)
{
    (void)c;
    (void)token;
	(void)Args;

	if (!state && channel.modeIsSet(CHANNEL_MODES::SET_INVITE_ONLY))
	{
		channel.unsetMode(CHANNEL_MODES::SET_INVITE_ONLY);
		replyModeNotify(client, channel, "-i", "", server);
	}
	else if (state && !channel.modeIsSet(CHANNEL_MODES::SET_INVITE_ONLY))
	{
		channel.setMode(CHANNEL_MODES::SET_INVITE_ONLY);
		replyModeNotify(client, channel, "+i", "", server);
	}
}
void	handleTopic(bool state, char c, vector<string> &token, Channel &channel, Client &client, Server &server, std::vector<std::string> &Args)
{
    (void)c;
    (void)token;
	(void)Args;

	if (!state && channel.modeIsSet(CHANNEL_MODES::SET_TOPIC))
	{
		channel.unsetMode(CHANNEL_MODES::SET_TOPIC);
		replyModeNotify(client, channel, "-t", "", server);
	}
	else if (state && !channel.modeIsSet(CHANNEL_MODES::SET_TOPIC))
	{
		channel.setMode(CHANNEL_MODES::SET_TOPIC);
		replyModeNotify(client, channel, "+t", "", server);
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
	channel.broadcastMessageToGroup(message, channel.getMembers(), client.getNick());
}


void	handleLimit(bool state, char c, vector<string> &token, Channel &channel, Client &client, Server &server, std::vector<std::string> &Args)
{
    (void)c;


	if (state && token.size() < 4)
		return Errors::ERR_NEEDMOREPARAMS(token[0], client, server);

	if (!state && channel.modeIsSet(CHANNEL_MODES::SET_LIMIT))
	{
		channel.unsetMode(CHANNEL_MODES::SET_LIMIT);
		replyModeNotify(client, channel, "-l", Utility::toStr(channel.getLimit()), server);
	}
	else if (state && (!channel.modeIsSet(CHANNEL_MODES::SET_LIMIT) || channel.modeIsSet(CHANNEL_MODES::SET_LIMIT)))
	{
		string str;
		if (Args[0][0] != ':')
		{
			str = Args[0];
			Args.erase(Args.begin());
		}

		std::stringstream ss(str);
		unsigned long     limit;
		ss >> limit;

		if (ss.fail() || !ss.eof())
			return Errors::CUSTOM_INVALID_LIMIT(client);

		channel.setLimit(limit);
		channel.setMode(CHANNEL_MODES::SET_LIMIT);

		replyModeNotify(client, channel, "+l", Utility::toStr(limit), server);
	}
}

void	handleKey(bool state, char c, vector<string> &token, Channel &channel, Client &client, Server &server, std::vector<std::string> &Args)
{
    (void)c;


	// MODE #D -k :password
	// 0     1  2    3
	if (token.size() < 4)
		return Errors::ERR_NEEDMOREPARAMS(token[0], client, server);



	string pass;
	if (Args[0][0] != ':')
	{
		pass = Args[0];
		Args.erase(Args.begin());
	}

	if (!state && channel.modeIsSet(CHANNEL_MODES::SET_KEY))
	{
		if (pass != channel.getPassword())
			return Errors::ERR_KEYALREADYSET(client.getNick(), channel.getChannelName(), client, server); // 467 client #channel :Channel key already set from adrift.sg.quakenet.org
		channel.unsetMode(CHANNEL_MODES::SET_KEY);
		replyModeNotify(client, channel, "-k", pass, server);
	}
	else if (state && (!channel.modeIsSet(CHANNEL_MODES::SET_KEY) || channel.modeIsSet(CHANNEL_MODES::SET_KEY)))
	{

		channel.setPassword(pass);

		channel.setMode(CHANNEL_MODES::SET_KEY);

		replyModeNotify(client, channel, "+k", pass, server);
	}
}

void	handleOperator(bool state, char c, vector<string> &token, Channel &channel, Client &client, Server &server, std::vector<std::string> &Args)
{
    (void)c;

	if (token.size() < 4)
		return Errors::ERR_NEEDMOREPARAMS(token[0], client, server);

	string nickToOp;
	if (Args[0][0] != ':')
	{
		nickToOp = Args[0];
		Args.erase(Args.begin());
	}

	if (!server.hasMember(nickToOp))
		return Errors::ERR_NOSUCHNICK(nickToOp, client, server);

	if (!channel.hasMember(nickToOp))
		return Errors::ERR_USERNOTINCHANNEL(nickToOp, channel.getChannelName(), client, server);

	Client	*clientToOp = channel.getMembers()[nickToOp];

	if (!state && channel.isOperator(clientToOp->getNick()))
	{
		channel.removeOperator(*clientToOp);

		replyModeNotify(client, channel, "-o", nickToOp, server);
	}
	else if (state && !channel.isOperator(clientToOp->getNick()))
	{
		channel.addOperator(*clientToOp);

		replyModeNotify(client, channel, "+o", nickToOp, server);
	}

}

void	HandleFlags(std::string& modeString, std::vector<std::string>& token, Channel& channel, Client &client, Server &server, std::vector<std::string> &Args)
{
	bool state = true;


	if (modeString[0] == '+' || modeString[0] == '-')
	{
		state = (modeString[0] == '+');
		modeString.erase(0, 1);
	}
	else
		return Errors::ERR_UNKNOWNMODE(string(1, modeString[0]), client, server);

	void (*f[])(bool state, char c, std::vector<std::string>& tmp, Channel& channel, Client &client, Server &server, std::vector<std::string> &Args) = {
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
		f[index](state, modeString[i], token, channel, client, server, Args);
	}
	modeString.erase(0, modeString.find_first_of("-+"));
}


void	Server::sendChannelModeToClient(Channel &channelObj, Client &client)
{
	string modeString = "+";
	if (channelObj.modeIsSet(CHANNEL_MODES::SET_INVITE_ONLY))
		modeString += "i";
	if (channelObj.modeIsSet(CHANNEL_MODES::SET_TOPIC))
		modeString += "t";
	if (channelObj.modeIsSet(CHANNEL_MODES::SET_LIMIT))
		modeString += "l";
	if (channelObj.modeIsSet(CHANNEL_MODES::SET_KEY))
		modeString += "k";

	Replies::RPL_CHANNELMODEIS(channelObj, client, modeString, *this); // 324
	Replies::RPL_CREATIONTIME (channelObj.getChannelName(), channelObj.getCreationTime(), client, *this); // 329
}

void	Server::applyModeToChannel(Channel &channelObj, Client &client, vector<string> &tokens)
{
	if (!channelObj.isOperator(client.getNick()))
			return Errors::ERR_CHANOPRIVSNEEDED(channelObj.getChannelName(), client, *this);

	string modeString;
	modeString = tokens[2];

	std::vector<std::string> Args(tokens.begin() + 3, tokens.end());
	while(!modeString.empty())
		HandleFlags(modeString, tokens, channelObj, client, *this, Args);
}


void Server::handleMODE(Client &client, vector<string> &tokens)
{
	size_t tokens_len = tokens.size();

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

	if (tokens_len == 2)
		sendChannelModeToClient(channelObj, client);
	else
		applyModeToChannel(channelObj, client, tokens);
}

time_t Server::getCreationTime()
{
	return this -> creationTime;
}

// Exceptions
const char *Server::TooManyChannels::what() const throw()
{
    return ("Server Exception: Too many channels");
}

const char *Server::CannotJoin::what() const throw()
{
    return ("Server Exception: Cannot Join channel");
}
