#ifndef Replies_HPP
# define Replies_HPP

#include<Hacks.hpp>
#include<Client.hpp>
#include<Server.hpp>

class Server;
class Client;

class Replies {
public:
	static void RPL_WELCOME(Client &client, Server &server);

private:
	Replies();
	Replies(const Replies &);
	Replies &operator=(const Replies &);
	~Replies();
};

#endif
