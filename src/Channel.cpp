#include <Channel.hpp>

Channel::Channel(): name("default") // op("default")
{
	//std::cout << "Channel: Default constructor called" << endl;
}

// TODO : add operators
Channel::Channel(string _name, string _password): name(_name), password(_password) //, op(NULL)
{
	//std::cout << "Channel: Parameter constructor called" << endl;
}


// getters
const string& Channel::getChannelName() const
{
	return (this -> name);
}

void Channel::addMember(Client &client)
{
	// TODO : maybe try to make this a reference later?
	// cerr << "Addr = " << &client << endl;
	members[client.getNick()] = &client;
	// cerr << client.getNick().size() << endl;
	cout << client.getNick() << " was added to channel " << getChannelName() << endl;
}

void Channel::removeMember(string &nick)
{
	if (!hasMember(nick))
		return ;
	members.erase(nick);
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
	if (find(chanOps.begin(), chanOps.end(), &client) != chanOps.end())
	{
		return; // TODO : do we need to send an error or something here (e.g trying re-make an op)
	}
	chanOps.push_back(&client);
}

void Channel::broadcast(string message)
{
	map<string, Client*>::iterator member_it = members.begin();
	map<string, Client*>::iterator member_ite = members.end();
	
	message += "\r\n";

	for (; member_it != member_ite; member_it++)
	{
		member_it -> second -> getFdObject() << message;
	}
}

bool Channel::hasMember(string &nick) {
	return (members.find(nick) != members.end());
}
