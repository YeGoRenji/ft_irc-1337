/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FD.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 11:54:34 by afatimi           #+#    #+#             */
/*   Updated: 2024/08/24 22:53:01 by afatimi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <FD.hpp>

FD::FD(): fd(-1)
{
	// std::cout << "FD: Default constructor called" << endl;
}

FD::FD(int _fd): fd(_fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
	// std::cout << "FD: Parameter constructor called" << endl;
}

FD &FD::operator=(const FD &obj)
{
	// std::cout << "FD: Copy assignment operator called" << endl;
	if (this != &obj)
	{
		this->fd = obj.fd;
		fcntl(fd, F_SETFL, O_NONBLOCK);
	}
	return (*this);
}

FD::FD(FD const &obj)
{
	// std::cout << "FD: Copy constructor called" << endl;
	if (this != &obj)
		*this = obj;
}

FD::~FD()
{
	// std::cout << "FD: Destructor called" << endl;
}

FD &FD::operator<<(std::string str) {
	str += "\r\n";
	write(fd, str.c_str(), str.size());
	return *this;
}

bool FD::operator>>(std::string& buffer) {
	char byte;

	buffer.clear();

	size_t buffSize = buffer.size();
	while (buffSize < 2 || !(buffer[buffSize - 2] == '\r' && buffer[buffSize - 1] == '\n') )
	{
		int byteRead = read(fd, &byte, 1);
		//cerr << "READ <" << byte << ">" << endl;
		//cerr << "byteRead = " << byteRead << endl;
		if (byteRead == 0)
			return true;
		if (byteRead == -1)
		{
			break;
		}
		buffer += byte;
		// l = buffer.size();
		buffSize += byteRead;
	}

	return false;
//	buffer.erase(buffer.find("\r\n"));
}

int FD::getValue() {
	return this->fd;
}

void FD::setValue(int fd) {
	this->fd = fd;
	if (fd >= 0)
		fcntl(fd, F_SETFL, O_NONBLOCK);
}
