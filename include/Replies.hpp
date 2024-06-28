#ifndef Replies_HPP
# define Replies_HPP

#include<Hacks.hpp>
#include<Client.hpp>
#include<Server.hpp>

//#define SIMPLE_REPLIES
//	X(001, WELCOME, "Welcome to the jeffy Network, ")

class Server;
class Client;

class Replies {
public:
	static void RPL_WELCOME(Client &client, Server &server);
	static void RPL_YOURHOST(Client &client, Server &server);
	static void RPL_CUSTOM_CLIENT_JOINED(string channel, Client &joiner, Client &client);
	static void RPL_INVITING(string &nick, string &channel, Client &client, Server &server);
	static void RPL_NOTOPIC(string &channel, Client &client, Server &server);
	static void RPL_TOPIC(string &channel, string &topic, Client &client, Server &server);
	static void RPL_TOPICWHOTIME(string &channel, string &setter, time_t time, Client &client, Server &server);

	static void notifyInvite(Client &inviter, Client &invited, string &channelName);

private:
	Replies();
	Replies(const Replies &);
	Replies &operator=(const Replies &);
	~Replies();
};

#endif
