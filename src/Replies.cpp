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
	reply += "\r\n";

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
// 	reply += "\r\n";

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
	reply += "\r\n";

	client << reply;
}

void Replies::RPL_TOPIC(Channel &channel, Client &client, Server &server)
{
	string reply = ":";
	reply += server.getServerName();
	reply += " 332 ";
	reply += Utility::getClientName(client, server);
	reply += " ";
	reply += channel.getChannelName();
	reply += " :";
	reply += channel.getTopic();
	reply += "\r\n";

	client << reply;
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
	reply += Utility::constructMemberList(channel.getMembers(), "");
	reply += Utility::constructMemberList(channel.getChanOps(), "@");
	reply += "\r\n";

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
	reply += "\r\n";

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
	reply += "\r\n";
  
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
	reply += "\r\n";
  
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
	reply += "\r\n";
  
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
	reply += std::to_string(time); // ila kant forbidden nbedlooha 
	reply += "\r\n";
  
	fd << reply;
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
	reply += "\r\n";
  
	invited << reply;
}