#include<Replies.hpp>

void Replies::RPL_WELCOME(Client &client, Server &server)
{
	string reply = ":";
	reply += server.getServerName();
	reply += " 001 ";
	reply += Utility::getClientName(client, server);
	reply += " :Welcome to ";
	reply += Server::serverName;
	reply += " Network, ";
	reply += client.getNick();

	client << reply;
}

void Replies::RPL_CREATED(Client &client, Server &server)
{
	string reply = ":";
	reply += server.getServerName();
	reply += " 003 ";
	reply += Utility::getClientName(client, server);
	reply += " :This server was created ";
	reply += Utility::getTimeFromEpoch(server.getCreationTime());

	client << reply;
}

void Replies::RPL_MYINFO(Client &client, Server &server)
{
	string reply = ":";
	reply += server.getServerName();
	reply += " 004 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += server.getServerName();
	reply += " ";
	reply += "6.9";
	reply += " ";
	reply += "iklot";

	client << reply;
}

void Replies::RPL_ISUPPORT(Client &client, Server &server)
{
	string reply = ":";

	reply += server.getServerName();
	reply += " 005 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += "USERIP";
	reply += " ";
	reply += "CHANTYPES=#";
	reply += " ";
	reply += "CASEMAPPING=ascii";
	reply += " ";
	reply += "PREFIX=(o)@";
	reply += " ";
	reply += "NICKLEN=";
	reply += Utility::toStr(Server::MAXNICKLEN);
	reply += " ";
	reply += "LOCATION=YOURMOM";
	reply += " ";
	reply += ":are supported by this server";
	// TODO : add MAXTOPICLEN & CHANLEN & MAXCHANNELS

	client << reply;
}

// void Replies::RPL_CUSTOM_CLIENT_JOINED(string channel, Client &joiner, Client &client)
// {
// 	string reply = ":";
// 	reply += joiner.getNick();
// 	reply += "!";
// 	reply += joiner.getUsername();
// 	reply += "@";
// 	reply += joiner.getIp();
// 	reply += " ";
// 	reply += "JOIN";
// 	reply += " ";
// 	reply += channel;

// 	client << reply;
// }

void Replies::RPL_YOURHOST(Client &client, Server &server)
{
	string reply = ":";
	reply += server.getServerName();
	reply += " 002 ";
	reply += Utility::getClientName(client, server);
	reply += " :Your host is ";
	reply += Server::serverName;
	reply += ", running version 6.9";

	client << reply;
}

// void Replies::RPL_TOPIC(Channel &channel, Client &client, Server &server)
// {
// 	string reply = ":";
// 	reply += server.getServerName();
// 	reply += " 332 ";
// 	reply += Utility::getClientName(client, server);
// 	reply += " ";
// 	reply += channel.getChannelName();
// 	reply += " :";
// 	reply += channel.getTopic();

// 	client << reply;
// }


void Replies::RPL_NAMREPLY(Channel &channel, Client &client, Server &server)
{
	string reply = ":";
	reply += server.getServerName();
	reply += " 353 ";
	reply += Utility::getClientName(client, server);
	reply += " = ";
	reply += channel.getChannelName();
	reply += " :";
	reply += channel.getNonOpsStrList();
	reply += channel.getChanOpsStrList();

	client << reply;
}

void Replies::RPL_ENDOFNAMES(Channel &channel, Client &client, Server &server)
{
	string reply = ":";
	reply += server.getServerName();
	reply += " 366 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += channel.getChannelName();
	reply += " :End of /NAMES list";

  client << reply;
}

void Replies::RPL_INVITING(string &nick, string &channel, Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string reply = ":";
	reply += server.getServerName();
	reply += " 341 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += nick;
	reply += " ";
	reply += channel;

	fd << reply;
}

void	Replies::RPL_NOTOPIC(string &channel, Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string reply = ":";
	reply += server.getServerName();
	reply += " 331 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += channel;
	reply += " :No topic is set";

	fd << reply;
}

void	Replies::RPL_TOPIC(string &channel, string &topic, Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string reply = ":";
	reply += server.getServerName();
	reply += " 332 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += channel;
	reply += " :";
	reply += topic;

	fd << reply;
}

void	Replies::RPL_TOPICWHOTIME(string &channel, string &setter, time_t time, Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string reply = ":";
	reply += server.getServerName();
	reply += " 333 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += channel;
	reply += " ";
	reply += setter;
	reply += " ";
	reply += Utility::toStr(time);

	fd << reply;
}

void Replies::RPL_PRIVMSG(Client &sender, Client &recevier, string &message)
{
	string reply = ":";

	reply += sender.getNick();
	reply += " PRIVMSG ";
	reply += recevier.getNick();
	reply += " :";
	reply += message;
	recevier << reply;
}

// NOTIFICATIONS
void Replies::notifyInvite(Client &inviter, Client &invited, string &channelName)
{
	string reply = ":";

	reply += inviter.getNick();
	reply += "!";
	reply += inviter.getUsername();
	reply += "@";
	reply += inviter.getIp();
	reply += " ";
	reply += "INVITE";
	reply += " ";
	reply += invited.getNick();
	reply += " ";
	reply += channelName;

	invited << reply;
}


void Replies::notifyKick(Client &kicker, Client &kicked, string &channelName)
{
	string reply = ":";

	reply += kicker.getNick();
	reply += "!";
	reply += kicker.getUsername();
	reply += "@";
	reply += kicker.getIp();
	reply += " ";
	reply += "KICK";
	reply += " ";
	reply += channelName;
	reply += " ";
	reply += kicked.getNick();

	kicked << reply;
}

void	Replies::RPL_CHANNELMODEIS(Channel &channelObj, Client &client, string &modeString, Server &server)
{
	string reply = ":";

	// cout << "MODE STRING : " << modeString << endl;
	reply += server.getServerName();
	reply += " ";
	reply += "324";
	reply += " ";
	reply += client.getNick();
	reply += " ";
	reply += channelObj.getChannelName();
	reply += " ";
	reply += modeString;

	// (void)channelObj;
	if (modeString.find("l") != string::npos)
	{
		// cout << "LIMIT : " << channelObj.getLimit() << endl;
		reply += " ";
		reply += Utility::toStr(channelObj.getLimit());
	}
	if (modeString.find("k") != string::npos)
	{
		reply += " ";
		// cout << "PASSWORD : " << channelObj.getPassword() << endl;
		if (channelObj.isOperator(client.getNick()))
			reply += channelObj.getPassword();
		else
			reply += "*";
	}

	cout << "REPLY : " << reply << endl;

	client << reply;
}

void	Replies::RPL_CREATIONTIME(string &channel, time_t time, Client &client, Server &server)
{
	string reply = ":";

	reply += server.getServerName();
	reply += " ";
	reply += "329";
	reply += " ";
	reply += client.getNick();
	reply += " ";
	reply += channel;
	reply += " ";
	reply += Utility::toStr(time);

	client << reply;
}
