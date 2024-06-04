#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>

int chk(int status, const std::string msg) {
	if (status < 0) {
		std::cerr << "Error[" << status << "]\t" << msg << std::endl;
		exit(status);
	}
	return status;
}

int main()
{
	int socketFd = chk(socket(AF_INET, SOCK_STREAM, 0), "Couldn't open socket");

	sockaddr_in address;
	bzero(&address, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_port   = htons(6969);
	address.sin_addr.s_addr = 0;

	chk(connect(socketFd, (sockaddr *)&address, sizeof(address)), "Couldn't connect");


	pollfd fds[2] = {
		{ 0,        POLLIN, 0 },
		{ socketFd, POLLIN,  0 },
	};


	while(69) {
		char buffer[256] = { 0 };

		poll(fds, 2, 69420);

		if (fds[0].revents & POLLIN) {
			read(0, buffer, 255);
			send(socketFd, buffer, 255, 0);
		} else if (fds[1].revents & POLLIN) {
			recv(socketFd, buffer, 255, 0);
			std::cout << "Server: " << buffer;
		}
	}

	// chk(connect(socketFd, (sockaddr *)&address, sizeof(address)), "Couldn't connect");

	// write(socketFd, "Noice", 5);

	return 0;
}
