/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FD.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 11:54:34 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/22 14:51:07 by ylyoussf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <FD.hpp>

FD::FD(): fd(-1)
{
	// std::cout << "FD: Default constructor called" << endl;
}

FD::FD(int _fd): fd(_fd)
{
	// std::cout << "FD: Parameter constructor called" << endl;
}

FD &FD::operator=(const FD &obj)
{
	// std::cout << "FD: Copy assignment operator called" << endl;
	if (this != &obj)
	{
		this->fd = obj.fd;
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
	write(fd, str.c_str(), str.size());
	return *this;
}

void FD::operator>>(std::string& str) {
	char buffer[1024] = { 0 };
	// TODO: implement 
	str.clear();
	int bytesRead = read(fd, buffer, sizeof(buffer) - 1);
	if (bytesRead <= 0)
		return; // TODO: Need to more stuff in this case
	buffer[bytesRead] = 0;
	str += buffer;
	cout << str;
	str.erase(find(str.begin(), str.end(), '\r'), str.end());
}

int FD::getValue() {
	return this->fd;
}

void FD::setValue(int fd) {
	this->fd = fd;
}
