#ifndef Channel_HPP
# define Channel_HPP

#include "Client.hpp"
# include "Hacks.hpp"

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
	map<string, Client*> members;
	// Client* op;
	vector<Message> messages;

};

#endif
