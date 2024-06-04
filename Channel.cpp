#include "Channel.hpp"

Channel::Channel()
{
	std::cout << "Channel: Default constructor called" << std::endl;
}

Channel::Channel(string name): name(name)//, op(NULL)
{
	std::cout << "Channel: Parameter constructor called" << std::endl;
}

Channel::~Channel()
{
	std::cout << "Channel: Destructor called" << std::endl;
}

