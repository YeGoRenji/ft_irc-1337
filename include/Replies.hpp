#ifndef Replies_HPP
# define Replies_HPP

#include<Hacks.hpp>
#include<Client.hpp>
#include<Server.hpp>
#include<ctime>

//#define SIMPLE_REPLIES
//	X(001, WELCOME, "Welcome to the jeffy Network, ")

class Server;
class Client;
class Channel;

class Replies {
public:
	static void RPL_WELCOME(Client &client, Server &server);
	static void RPL_CREATED(Client &client, Server &server);
	static void RPL_YOURHOST(Client &client, Server &server);
	static void RPL_MYINFO(Client &client, Server &server);
	static void RPL_ISUPPORT(Client &client, Server &server);
	static void RPL_MOTDSTART(Client &client, Server &server);
	static void RPL_MOTD(Client &client, Server &server, string line);
	static void RPL_ENDOFMOTD(Client &client, Server &server);
	static void sendBanner(Client &client, Server &server);


	static void RPL_CUSTOM_CLIENT_JOINED(string channel, Client &joiner, Client &client);
	static void RPL_INVITING(string &nick, string &channel, Client &client, Server &server);
	static void RPL_NOTOPIC(string &channel, Client &client, Server &server);
	static void RPL_TOPIC(string &channel, string &topic, Client &client, Server &server);
	static void RPL_TOPICWHOTIME(string &channel, string &setter, time_t time, Client &client, Server &server);
	static void RPL_NAMREPLY(Channel &channel, Client &client, Server &server);
	static void RPL_ENDOFNAMES(Channel &channel, Client &client, Server &server);
	static void RPL_PRIVMSG(Client &sender, Client &recevier, string &message);

	static void	RPL_CHANNELMODEIS(Channel &channelObj, Client &client, string &modeString, Server &server);
	static void	RPL_CREATIONTIME(string &channel, time_t time, Client &client, Server &server);


	static void notifyInvite(Client &inviter, Client &invited, string &channelName);
	static void notifyKick(Client &kicker, Client &kicked, string &channelName);

private:
	Replies();
	Replies(const Replies &);
	Replies &operator=(const Replies &);
	~Replies();
};

#endif
