/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FD.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylyoussf <ylyoussf@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 11:54:34 by afatimi           #+#    #+#             */
/*   Updated: 2024/09/05 19:53:35 by ylyoussf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <FD.hpp>

FD::FD(): fd(-1)
{

}

FD::FD(int _fd): fd(_fd)
{

}

FD &FD::operator=(const FD &obj)
{

	if (this != &obj)
	{
		this->fd = obj.fd;
	}
	return (*this);
}

FD::FD(FD const &obj)
{

	if (this != &obj)
		*this = obj;
}

FD::~FD()
{

}

FD &FD::operator<<(std::string str) {
	str += "\r\n";
	write(fd, str.c_str(), str.size());
	return *this;
}

void FD::operator>>(std::string& buffer) {
	char byte;

	buffer.clear();

	size_t l = buffer.size();
	while (l < 2 || !(buffer[l - 2] == '\r' && buffer[l - 1] == '\n') )
	{
		int byteRead = read(fd, &byte, 1);

		if (byteRead == -1)
		{

			break;
		}
		if (byteRead)
			buffer += byte;
		l = buffer.size();
	}

	if (CONTAINS(buffer, "\r\n"))
		buffer.erase(buffer.find("\r\n"));
}

int FD::getValue() {
	return this->fd;
}

void FD::setValue(int fd) {
	this->fd = fd;
}
