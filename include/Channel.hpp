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
	void removeMember(string &nick);
	bool checkPassword(string userPassLine);
	bool hasPassword() const;
	bool hasMember(string &nick);
	void addOperator(Client &client);
	void broadcastMessage(string message);
	void broadcastJoiner(Client &joiner);
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
