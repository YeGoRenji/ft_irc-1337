#ifndef Replies_HPP
# define Replies_HPP

#include<Hacks.hpp>
#include<Client.hpp>
#include<Server.hpp>

//#define SIMPLE_REPLIES
//	X(001, WELCOME, "Welcome to the jeffy Network, ")

class Server;
class Client;
class Channel;

class Replies {
public:
	static void RPL_WELCOME(Client &client, Server &server);
	static void RPL_YOURHOST(Client &client, Server &server);
	static void RPL_TOPIC(Channel &channel, Client &client, Server &server);
	static void RPL_NAMREPLY(Channel &channel, Client &client, Server &server);
	static void RPL_ENDOFNAMES(Channel &channel, Client &client, Server &server);
	// static void RPL_CUSTOM_CLIENT_JOINED(string channel, Client &joiner, Client &client);

private:
	Replies();
	Replies(const Replies &);
	Replies &operator=(const Replies &);
	~Replies();
};

#endif
