#ifndef Channel_HPP
# define Channel_HPP

# include "Client.hpp"
# include "Hacks.hpp"

class Client;

enum BroadCastAction {
	JOIN,
	PART
};

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
	void removeMember(Client &client, string reason);
	bool checkPassword(string userPassLine);
	bool hasPassword() const;
	bool hasMember(string &nick);
	void addOperator(Client &client);
	void broadcastMessage(string message);
	void broadcastAction(Client &client, string reason, BroadCastAction action);
	bool isOperator(string &nick);
	static bool isValidName(string &name);
	~Channel();
private:
	string name;
	string topic; // TODO how to set the topic??

	map<string, Client*> members;
	vector<Client*> chanOps;
	// password
	string password;
	vector<Message> messages;
};

#endif
