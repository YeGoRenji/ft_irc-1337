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
	reply += Utility::toStr(Server::NICKLEN);
	reply += " ";
	reply += "TOPICLEN=";
	reply += Utility::toStr(Server::TOPICLEN);
	reply += " ";
	reply += "CHANNELLEN=";
	reply += Utility::toStr(Server::CHANNELLEN);
	reply += " ";
	reply += "MAXCHANNELS=";
	reply += Utility::toStr(Server::MAXCHANNELS);
	reply += " ";
	reply += "MAXCLIENTS=";
	reply += Utility::toStr(Server::MAXCLIENTS);
	reply += " ";
	reply += "LOCATION=YOURMOM";
	reply += " ";
	reply += ":are supported by this server";

	client << reply;
}

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

void Replies::RPL_MOTD(Client &client, Server &server, string line)
{
	string reply = ":";
	reply += server.getServerName();
	reply += " 372 ";
	reply += Utility::getClientName(client, server);
	reply += " :";
	reply += line;

	client << reply;
}

void Replies::RPL_MOTDSTART(Client &client, Server &server) {
	string reply = ":";
	reply += server.getServerName();
	reply += " 375 ";
	reply += Utility::getClientName(client, server);
	reply += " :- ";
	reply += server.getServerName();
	reply += " Message of the day - ";

	client << reply;
}

void Replies::RPL_ENDOFMOTD(Client &client, Server &server) {
	string reply = ":";
	reply += server.getServerName();
	reply += " 376 ";
	reply += Utility::getClientName(client, server);
	reply += " :End of /MOTD command.";

	client << reply;
}

void Replies::sendBanner(Client &client, Server &server) {
	RPL_MOTDSTART(client, server);

	RPL_MOTD(client, server, "._. ._____.           ._. ._.                                      ._.");
	RPL_MOTD(client, server, "| | |_   _|           | | | |                                      | |");
	RPL_MOTD(client, server, "| |   | |    _ __ __ _| |_| |__   ___ _ __    ___ _   _ _ __ ___   | |");
	RPL_MOTD(client, server, "| |   | |   | '__/ _` | __| '_ \\ / _ \\ '__|  / __| | | | '_ ` _ \\  | |");
	RPL_MOTD(client, server, "| |  _| |_  | | | (_| | |_| | | |  __/ |    | (__| |_| | | | | | | | |");
	RPL_MOTD(client, server, "| | |_____| |_|  \\__,_|\\__|_| |_|\\___|_|     \\___|\\__,_|_| |_| |_| | |");
	RPL_MOTD(client, server, "| |                                                                | |");
	RPL_MOTD(client, server, "| |       This server was made while gooning by :                  | |");
	RPL_MOTD(client, server, "| |          o YeGo  (https://github.com/YeGoRenji)                | |");
	RPL_MOTD(client, server, "| |          o Jeffy (https://github.com/0x00Jeff)                 | |");
	RPL_MOTD(client, server, "| |          o SkrkL (https://github.com/SkaRkaL)                  | |");
	RPL_MOTD(client, server, "|_|                                                                |_|");

	RPL_ENDOFMOTD(client, server);
}

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
		reply += Utility::toStr(channelObj.getLimit());
	}
	if (modeString.find("k") != string::npos)
	{
		reply += " ";
		if (channelObj.isOperator(client.getNick()))
			reply += channelObj.getPassword();
		else
			reply += "*";
	}
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
