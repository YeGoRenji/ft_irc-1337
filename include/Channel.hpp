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
	void addOperator(Client &client);
	void broadcast(string message);

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
