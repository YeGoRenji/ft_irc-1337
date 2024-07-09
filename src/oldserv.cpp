#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

	chk(bind(socketFd, (sockaddr *)&address, sizeof(address)), "Couldn't bind socket to address");

	chk(listen(socketFd, 128), "Couldn't Listen to socket");

	int clientFd = chk(accept(socketFd, NULL, NULL), "Couldn't accept connection 1");
	puts("Accepted Connection 1");
	int client2Fd = chk(accept(socketFd, NULL, NULL), "Couldn't accept connection 2");
	puts("Accepted Connection 2");


	pollfd fds[4] = {
		{ 0,        POLLIN, 0 },
		{ clientFd, POLLIN, 0 },
		{ client2Fd, POLLIN, 0 },
		{ socketFd, POLLIN, 0 }
	};

	while(69) {
		char buffer[256] = { 0 };

		poll(fds, 4, 69420);

		if (fds[0].revents & POLLIN) {
			read(0, buffer, 255);
			send(clientFd, buffer, 255, 0);
			send(client2Fd, buffer, 255, 0);
		}
		if (fds[1].revents & POLLIN) {
			recv(clientFd, buffer, 255, 0);
			std::cout << "Client1 : " << buffer;
		}
		if (fds[2].revents & POLLIN) {
			recv(client2Fd, buffer, 255, 0);
			std::cout << "Client2 : " << buffer;
		}
		if (fds[3].revents & POLLIN) {
			puts("Noice !");
		}
	}



	// chk(connect(socketFd, (sockaddr *)&address, sizeof(address)), "Couldn't connect");

	// write(socketFd, "Noice", 5);

	return 0;
}
