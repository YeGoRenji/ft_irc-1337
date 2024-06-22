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
	Channel();
	Channel(string name, string password); // TODO : add operators
	// getters
	const string& getChannelName() const; 

	void addMember(Client &client);
	bool checkPassword(string userPassLine);
	bool hasPassword() const;

	~Channel();
private:
	string name;
	string topic; // TODO how to set the topic??
				  
	// TODO : why the fuck do we have a string/client here 
	// NOTE : i decided to put the nickname here
	map<string, Client*> members;
	// password
	string password;
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
