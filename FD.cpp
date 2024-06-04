#include "FD.hpp"

FD::FD(): fd(-1)
{
	std::cout << "FdezNuts: Default constructor called" << std::endl;
}

FD::FD(int _fd): fd(_fd)
{
	std::cout << "FdezNuts: Parameter constructor called" << std::endl;
}

FD &FD::operator=(const FD &obj)
{
	std::cout << "FdezNuts: Copy assignment operator called" << std::endl;
	if (this != &obj)
	{
		this->fd = obj.fd;
	}
	return (*this);
}

FD::FD(FD const &obj)
{
	std::cout << "FdezNuts: Copy constructor called" << std::endl;
	if (this != &obj)
		*this = obj;
}

FD::~FD()
{
	close(fd);
	std::cout << "FdezNuts: Destructor called" << std::endl;
}

void FD::operator<<(std::string str) {
	write(fd, str.c_str(), str.size());
}

void FD::operator>>(std::string& str) {
	char buffer[1024] = { 0 };
	int result = 69;
	str.clear();
	while ((result = read(fd, buffer, sizeof(buffer))) > 0) {
		cout << "Got loop result: " << result << endl;
		str += buffer;
	}
	cout << "Got result: " << result << endl;
	if (result == EOF) {
		cout << "Disconnected lol" << endl;
		close(fd);
	}
}

int FD::getValue() {
	return this->fd;
}

void FD::setValue(int fd) {
	this->fd = fd;
}
