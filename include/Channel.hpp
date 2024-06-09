#ifndef Channel_HPP
# define Channel_HPP

# include "Client.hpp"
# include "Hacks.hpp"

class Client;

struct Message {
	Client *sender;
	string body;
};

class Channel {
public:
	Channel(string name);
	~Channel();

private:
	Channel();
	string name;
	string topic;
	map<string, Client*> members;
	// Client* chanOps;
	/* 
	 * Client * halfOps
	 * Client * founder
	 * TODO : do we need to support these ??
	 * src : https://modern.ircdocs.horse/#channel-operators
	*/
	vector<Message> messages;

};

#endif
