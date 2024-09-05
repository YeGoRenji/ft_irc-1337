/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FD.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sakarkal <sakarkal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 11:54:34 by afatimi           #+#    #+#             */
/*   Updated: 2024/09/05 19:54:50 by sakarkal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <FD.hpp>

FD::FD() : fd(-1) {}

FD::FD(int _fd) : fd(_fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

FD &FD::operator=(const FD &obj)
{
	if (this != &obj)
	{
		this->fd = obj.fd;
		fcntl(fd, F_SETFL, O_NONBLOCK);
	}
	return (*this);
}

FD::FD(FD const &obj)
{
	if (this != &obj)
		*this = obj;
}

FD::~FD() {}

FD &FD::operator<<(std::string str)
{
	str += "\r\n";
	write(fd, str.c_str(), str.size());
	return *this;
}

void FD::operator>>(std::string &buffer)
{
	char byte;

	buffer.clear();

	size_t buffSize = buffer.size();
	while (buffSize < 2 || !(buffer[buffSize - 2] == '\r' && buffer[buffSize - 1] == '\n'))
	{
		int byteRead = read(fd, &byte, 1);
		if (byteRead == 0)
			return;
		if (byteRead == -1)
			break;
		buffer += byte;
		buffSize += byteRead;
	}
}

int FD::getValue()
{
	return this->fd;
}

void FD::setValue(int fd)
{
	this->fd = fd;
	if (fd >= 0)
		fcntl(fd, F_SETFL, O_NONBLOCK);
}
