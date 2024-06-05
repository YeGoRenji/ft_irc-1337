#include "Utility.hpp"

bool Utility::match(stringstream &ss, string target)
{
	string keyword;
	ss >> keyword;
	return (keyword == target);
}

Utility::Utility()
{
	std::cout << "Utility: Default constructor called" << std::endl;
}

Utility &Utility::operator=(const Utility &obj)
{
	std::cout << "Utility: Copy assignment operator called" << std::endl;
	if (this != &obj) {}
	return (*this);
}

Utility::Utility(Utility const &obj)
{
	std::cout << "Utility: Copy constructor called" << std::endl;
	if (this != &obj)
		*this = obj;
}

Utility::~Utility()
{
	std::cout << "Utility: Destructor called" << std::endl;
}

