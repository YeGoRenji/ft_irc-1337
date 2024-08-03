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
	stringstream ss;
	ss << time;

	string reply = ":";
	reply += server.getServerName();
	reply += " 333 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += channel;
	reply += " ";
	reply += setter;
	reply += " ";
	reply += ss.str();

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
		reply += " ";
		// cout << "LIMIT : " << channelObj.getLimit() << endl;
		reply += std::to_string(channelObj.getLimit());
	}
	if (modeString.find("k") != string::npos)
	{
		reply += " ";
		// cout << "PASSWORD : " << channelObj.getPassword() << endl;
		reply += channelObj.getPassword();
	}

	cout << "REPLY : " << reply << endl;

	client << reply;
}

void	Replies::RPL_CREATIONTIME(string &channel, time_t time, Client &client, Server &server)
{
	string reply = ":";
	stringstream ss;
	ss << time;

	reply += server.getServerName();
	reply += " ";
	reply += "329";
	reply += " ";
	reply += client.getNick();
	reply += " ";
	reply += channel;
	reply += " ";
	reply += ss.str();

	client << reply;
}
