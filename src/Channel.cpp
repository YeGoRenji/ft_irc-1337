#include "Channel.hpp"

Channel::Channel(): name("default") // op("default")
{
	//std::cout << "Channel: Default constructor called" << endl;
}

// TODO : add operators
Channel::Channel(string _name, string _password): name(_name) //, op(NULL)
{
	this -> hasPassword = !_password.empty();
	if (this -> hasPassword)
		this -> password = _password;
	//std::cout << "Channel: Parameter constructor called" << endl;
}

// getters
const string& Channel::getChannelName() const
{
	return (this -> name);
}


void Channel::addMember(Client &client)
{
	members[client.getNick()] = &client;
}

Channel::~Channel()
{
	// TODO : Problem : this gets called a lot, figure out a better way
	//std::cout << "Channel: Destructor called" << endl;
}
