#ifndef Channel_HPP
# define Channel_HPP

# include "Client.hpp"
# include "Hacks.hpp"

class Client;
class Server;

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
	const string& getTopic() const;
	map<string, Client*> &getMembers();
	map<string, Client*> &getChanOps();
	size_t getMemberCount() const;
	size_t getChanOpCount() const;

	// setters
	void setTopic(string topic);

	// Methods
	void addMember(Client &client);
	void removeMember(Client &client, string reason);
	bool checkPassword(string userPassLine);
	bool hasPassword() const;
	bool hasMember(string &nick);
	void addOperator(Client &client);
	void broadcastMessage(string message);
	void broadcastAction(Client &client, string reason, BroadCastAction action);
	bool isOperator(string &nick);
	void sendClientsList(Channel &channel, Client &client, Server &server);
	static bool isValidName(string &name);

	~Channel();
private:
	string name;
	string topic;

	map<string, Client*> members;
	map<string, Client*> chanOps;
	// password
	string password;
	vector<Message> messages;
};

#endif
