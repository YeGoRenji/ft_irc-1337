#include <Channel.hpp>

Channel::Channel(): name("default") // op("default")
{
	//std::cout << "Channel: Default constructor called" << endl;
}

Channel::Channel(string _name, string _password, CHANNEL_MODES::Modes _mode):
name(Utility::toLower(_name)), creationTime(time(0)), password(_password), mode(_mode), limit(69)
{
	//std::cout << "Channel: Parameter constructor called" << endl;
}

void Channel::addMemberAndBroadcast(Client &client) {
	addMember(client, true);
}

void Channel::addMemberSilently(Client &client) {
	addMember(client, false);
}

void Channel::addMember(Client &client, bool isBroadcasted)
{
	// cerr << "Addr = " << &client << endl;
	members[client.getNick()] = &client;
	// cerr << client.getNick().size() << endl;
	cout << client.getNick() << " (" << &client << ") was added to channel " << getChannelName() << endl;
	if (isBroadcasted)
		broadcastAction(client, "", JOIN);
}

void Channel::removeMemberAndBroadcast(Client &client, string reason) {
	removeMember(client, reason, true);
}

void Channel::removeMemberSilently(Client &client) {
	removeMember(client, "69", false);
}

CHANNEL_MODES::Modes operator|(CHANNEL_MODES::Modes i, CHANNEL_MODES::Modes j)
{
	return static_cast<CHANNEL_MODES::Modes>(static_cast<int>(i) | static_cast<int>(j));
}

CHANNEL_MODES::Modes operator&(CHANNEL_MODES::Modes i, CHANNEL_MODES::Modes j)
{
	return static_cast<CHANNEL_MODES::Modes>(static_cast<int>(i) & static_cast<int>(j));
}

bool	Channel::modeIsSet(CHANNEL_MODES::Modes _mode) {
		return (this->mode & _mode);
}

void	Channel::setMode(CHANNEL_MODES::Modes _mode) {
	this->mode = this->mode | _mode;
}

void	Channel::unsetMode(CHANNEL_MODES::Modes _mode) {
	this->mode = static_cast<CHANNEL_MODES::Modes>(~_mode & mode);
}

void Channel::removeMember(Client &client, string reason, bool isBroadcasted)
{
	string &nick = client.getNick();
	if (!hasMember(nick))
		return ;

	if (isBroadcasted)
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
	return (userPassLine == this -> password);
}

Channel::~Channel()
{
	//std::cout << "Channel: Destructor called" << endl;
}

void Channel::addOperator(Client &client)
{
	if (chanOps.find(client.getNick()) != chanOps.end())
	{
		return;
	}
	chanOps[client.getNick()] = &client;
}

void Channel::removeOperator(Client &client)
{
    // Check if the client is already in the map
    if (chanOps.find(client.getNick()) != chanOps.end())
    {
        // Remove the client from the map
        chanOps.erase(client.getNick());
    }
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

	broadcastMessageToGroup(reply, getMembers(), "");
}

void Channel::broadcastMessageToGroup(string message, map<string, Client*> &group, string senderNick)
{
	map<string, Client*>::iterator member_it = group.begin();
	map<string, Client*>::iterator member_ite = group.end();

	for (; member_it != member_ite; member_it++)
	{
		Client *member = member_it->second;
		if (member -> getNick() == senderNick)
			continue;
		*member << message;
	}
}

bool Channel::hasMember(string &nick) {
	return (members.find(nick) != members.end());
}

bool Channel::isOperator(string &nick) {
	return (chanOps.find(nick) != chanOps.end());
}

bool Channel::canBeJoinedBy(Client &client, string suppliedPass, Server &server) {

	// Already in channel
	if (hasMember(client.getNick()))
		return false;

	// channel is invite only and Client not invited
	if (modeIsSet(CHANNEL_MODES::SET_INVITE_ONLY) && !isNickInvited(client.getNick()))
	{
		Errors::ERR_INVITEONLYCHAN(name, client, server);
		return false;
	}

	// You can invite people to a non-invite only channel
	// What priv it gives is :
	//  - it bypasses Limit
	//  - it bypasses Password
	if (isNickInvited(client.getNick()))
		return outvite(&client);

	// user supplied a wrong password
	if (modeIsSet(CHANNEL_MODES::SET_KEY) && !checkPassword(suppliedPass))
	{
		Errors::ERR_BADCHANNELKEY(name, client, server);
		return false;
	}

	// channel has limit and full
	if (modeIsSet(CHANNEL_MODES::SET_LIMIT) && getMemberCount() >= limit)
	{
		Errors::ERR_CHANNELISFULL(name, client, server);
		return false;
	}

	return true;
}

bool Channel::isValidName(string &name) {

	if (name.empty())
		return false;

	if (name[0] != '#')
		return false;

	if (name.size() == 1)
		return false;

	if (name.find_first_of(" ,\x07") != string::npos)
		return false;

	return true;
}

void Channel::addMessage(string sender, string message)
{
	messages.push_back((Message){sender, message});
}

void	Channel::setTopic(string &newTopic, string &setter)
{
	cerr << "Setting topic to " << newTopic << " by " << setter << endl;
	this->topic = newTopic;
	this->topicSetter = setter;
	this->topicSetTime = time(0);

	cout <<  "this->topicSetTime	" << this->topicSetTime << endl;
}

void Channel::sendClientsList(Client &client, Server &server)
{
	Replies::RPL_NAMREPLY(*this, client, server);
	Replies::RPL_ENDOFNAMES(*this, client, server);
}

// getters

string	&Channel::getTopic()
{
        return topic;
}

string	&Channel::getTopicSetter()
{
	return topicSetter;
}

time_t			Channel::getTopicSetTime()
{
	return topicSetTime;
}

string& Channel::getChannelName()
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

size_t Channel::getChanOpCount() const
{
	return (chanOps.size());
}

string Channel::getNonOpsStrList()
{
	string str;
	map<string, Client*>::iterator it = members.begin();
	map<string, Client*>::iterator ite = members.end();

	for (; it != ite; ++it)
	{
		if (!isOperator(it->second->getNick())) {
			str += it->second->getNick();
			str += " ";
		}
	}

	return str;
}

string Channel::getChanOpsStrList()
{
	return Utility::constructMemberList(getChanOps(), "@");
}

// setters

void Channel::setLimit(unsigned long _limit)
{
	this->limit = _limit;
}
unsigned long	Channel::getLimit(void)
{
	return (this->limit);
}

void Channel::invite(Client* client) {
    this->invited[client->getNick()] = client;
	cout << "[ Invited " << client->getNick() << " to " << this->name << " ]" << endl;
}

bool Channel::outvite(Client* client) {
	this->invited.erase(client->getNick());
	cout << "[ " << client->getNick() << " got outvited from " << this->name << " ]" << endl;
	return true;
}

bool Channel::isNickInvited(string nick) {
	return (invited.find(nick) != invited.end());
}

const string	&Channel::getPassword() const {
    return this->password;
}

void	Channel::setPassword(const string &_password) {
	cout << "\n\n\nSetting keyy to " << _password << endl;
	cout << "Current keyy is " << this->password << "\n\n\n\n";
    this->password = _password;
}

time_t	Channel::getCreationTime() const
{
	return creationTime;
}
