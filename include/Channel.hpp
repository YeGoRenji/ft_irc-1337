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
	string sender;
	string body;
	// TODO : might wanna add the time here
};

class Channel {
public:
	Channel();
	Channel(string name, string password); // TODO : add operators
	// getters
	string& getChannelName() ;
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
	void broadcastMessageToGroup(string message, map<string, Client*> &group, string senderName);
	void broadcastAction(Client &client, string reason, BroadCastAction action);
	bool isOperator(string &nick);
	void sendClientsList(Channel &channel, Client &client, Server &server);
	void addMessage(string sender, string message);
	static bool isValidName(string &name);

	void	setTopic(string& newTopic, string& setter);

	string	&getTopic() {
        return topic;
    }

    string	&getTopicSetter() {
        return topicSetter;
    }

    time_t			getTopicSetTime() {
        return topicSetTime;
    }

	// void invite(Client* client);

	~Channel();
private:
	string					name;
	string					topic;
	string					topicSetter;
  time_t					topicSetTime;

	// std::vector<Client*>	_invited; // TODO : zedtha gelt maybe nahtajohaa

	map<string, Client*>	members;
	map<string, Client*> chanOps;
	// password
	string					password;
	vector<Message>			messages;
};

#endif
