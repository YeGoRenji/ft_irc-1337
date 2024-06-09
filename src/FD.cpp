/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FD.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 11:54:34 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/10 00:19:49 by afatimi          ###   ########.fr       */
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
	str.clear();
	read(fd, buffer, sizeof(buffer));
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
