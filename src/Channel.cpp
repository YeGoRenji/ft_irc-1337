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
	members[client.getNick()] = &client;
	cout << client.getNick() << " was added to channel " << getChannelName() << endl;
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
