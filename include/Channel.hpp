#ifndef Channel_HPP
# define Channel_HPP

# include "Client.hpp"
# include "Hacks.hpp"

class Client;
class Server;

namespace CHANNEL_MODES {
	enum Modes {
		NO_MODE = 0b0,
		SET_INVITE_ONLY = 0b10,
		SET_TOPIC = 0b100,
		SET_KEY = 0b1000,
		GIVE_PRIVILEGE = 0b10000,
		SET_LIMIT = 0b100000,
	};
}


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
	Channel(string name, string password, CHANNEL_MODES::Modes _mode); // TODO : add operators
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
	void broadcastMessage(string message);
	void broadcastAction(Client &client, string reason, BroadCastAction action);
	bool isOperator(string &nick);
  void sendClientsList(Channel &channel, Client &client, Server &server);
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


	bool	modeIsSet(CHANNEL_MODES::Modes _mode);

	void	setMode(CHANNEL_MODES::Modes _mode);

	void	unsetMode(CHANNEL_MODES::Modes _mode);



private:
	string					name;

	map<string, Client*>	members;
	map<string, Client*> chanOps;
	// password
	string					password;
	vector<Message>			messages;

	// std::vector<Client*>	_invited; // TODO : zedtha gelt maybe nahtajohaa
	string					topicSetter;
	string					topic;
	CHANNEL_MODES::Modes		mode;
	time_t					topicSetTime;
	// uint16_t				limit;

};

CHANNEL_MODES::Modes operator|(CHANNEL_MODES::Modes i, CHANNEL_MODES::Modes j);

CHANNEL_MODES::Modes operator&(CHANNEL_MODES::Modes i, CHANNEL_MODES::Modes j);



#endif
