#include <Channel.hpp>

Channel::Channel(): name("default") // op("default")
{
	//std::cout << "Channel: Default constructor called" << endl;
}

// TODO : add operators
Channel::Channel(string _name, string _password): name(_name), topic("+65-59"), password(_password) // TODO: remove topic()
{
	//std::cout << "Channel: Parameter constructor called" << endl;
}


void Channel::addMember(Client &client)
{
	// TODO : maybe try to make this a reference later?
	// cerr << "Addr = " << &client << endl;
	members[client.getNick()] = &client;
	// cerr << client.getNick().size() << endl;
	cout << client.getNick() << " was added to channel " << getChannelName() << endl;
	broadcastAction(client, "", JOIN);
}

void Channel::removeMember(Client &client, string reason)
{
	string &nick = client.getNick();
	if (!hasMember(nick))
		return ;

	broadcastAction(client, reason, PART);
	members.erase(nick);
	chanOps.erase(nick);
}

bool Channel::hasPassword() const
{
	return (!password.empty());
}

bool Channel::checkPassword(string userPassLine)
{
	if (password.empty())
		return (true);
	return (userPassLine == this -> password);
}

Channel::~Channel()
{
	// TODO : Problem : this gets called a lot, figure out a better way
	//std::cout << "Channel: Destructor called" << endl;
}

void Channel::addOperator(Client &client)
{
	if (chanOps.find(client.getNick()) != chanOps.end())
	{
		return; // TODO : do we need to send an error or something here (e.g trying re-make an op)
	}
	chanOps[client.getNick()] = &client;
}

void Channel::broadcastAction(Client &client, string reason, BroadCastAction action)
{
	string reply = ":";
	const char *toStr[2] = {
		"JOIN",
		"PART"
	};

	reply += client.getNick();
	reply += "!";
	reply += client.getUsername();
	reply += "@";
	reply += client.getIp();
	reply += " ";
	reply += toStr[action];
	reply += " ";
	reply += this -> name;
	if (!reason.empty()) {
		reply += " ";
		if (reason.find(' ') != string::npos)
			reply += ":";
		reply += reason;
	}
	cerr << "BROADCASTING <" << reply << ">" << endl;
	reply += "\r\n";

	broadcastMessage(reply);
}

void Channel::broadcastMessage(string message)
{
	map<string, Client*>::iterator member_it = members.begin();
	map<string, Client*>::iterator member_ite = members.end();

	for (; member_it != member_ite; member_it++)
	{
		*(member_it -> second) << message;
	}
}

bool Channel::hasMember(string &nick) {
	return (members.find(nick) != members.end());
}

bool Channel::isOperator(string &nick) {
	return (chanOps.find(nick) != chanOps.end());
}

bool Channel::isValidName(string &name) {

	if (name.empty())
		return false;

	if (name[0] != '#')
		return false;

	if (name.find_first_of(" ,\x07") != string::npos)
		return false;

	return true;
}

void Channel::sendClientsList(Channel &channel, Client &client, Server &server)
{
	Replies::RPL_NAMREPLY(channel, client, server);
	Replies::RPL_ENDOFNAMES(channel, client, server);
}

// getters
const string& Channel::getChannelName() const
{
	return (this -> name);
}

const string& Channel::getTopic() const
{
	return (this -> topic);
}

map<string, Client*> &Channel::getMembers() {
	return (this -> members);
}

map<string, Client*> &Channel::getChanOps() {
	return (this -> chanOps);
}

size_t Channel::getMemberCount() const
{
	return (members.size());
}

// setters
void Channel::setTopic(string topic)
{
	this -> topic = topic;
}
