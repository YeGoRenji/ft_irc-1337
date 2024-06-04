#include "Server.hpp"
#include "Client.hpp"
#include <cstdio>
#include <sys/socket.h>

int chk(int status, const std::string msg) {
	// TODO: use throw ?
	if (status < 0) {
		std::cerr << "Error[" << status << "]\t" << msg << std::endl;
		perror("Hh: ");
		exit(status);
	}
	return status;
}

Server::Server(int port)
{
	std::cout << "Server: Parameter constructor called" << std::endl;

	serverSocket.setValue(chk(socket(AF_INET, SOCK_STREAM, 0), "Couldn't open socket"));

	cout << "Server socket : " << serverSocket.getValue() << endl;

	int yes = 1;

	chk(setsockopt(serverSocket.getValue(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)), "Couldn't set socket option");

	sockaddr_in address;
	bzero(&address, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_port   = htons(port);
	address.sin_addr.s_addr = 0;

	chk(bind(serverSocket.getValue(), (sockaddr *)&address, sizeof(address)), "Couldn't bind socket to address");

	chk(listen(serverSocket.getValue(), 10), "Couldn't Listen to socket");
}

Server::~Server()
{
	std::cout << "Server: Destructor called" << std::endl;
}

void Server::start() {

	while(69) {
		pollfd fds[1 + clients.size()];

		fds[0] = (pollfd){ serverSocket.getValue(), POLLIN, 0 };
		for (size_t i = 0; i < clients.size(); ++i) {
			fds[i + 1] = (pollfd){ clients[i]->getFd(), POLLIN, 0 };
		}


		// char buffer[256] = { 0 };

		poll(fds, 1 + clients.size(), -1);

		if (fds[0].revents & POLLIN) {
			int newClient = chk(accept(serverSocket.getValue(), NULL, NULL), "Couldn't accept connection");

			clients.push_back(new Client(newClient));

			cout << "New client connected " << newClient << ", Size = " << 1 + clients.size() << endl;
			fds[0].revents = 0;
		}

		for (size_t i = 1; i < 1 + clients.size(); ++i) {
			if (fds[i].revents & POLLIN) {
				string data;
				clients[i - 1]->getFdObject() >> data;
				cout << "Got <" << data << ">" << endl;
				fds[i].revents = 0;
			}
		}

		// if (fds[1].revents & POLLIN) {
		// 	recv(clientFd, buffer, 255, 0);
		// 	std::cout << "Client1 : " << buffer;
		// }
		// if (fds[2].revents & POLLIN) {
		// 	recv(client2Fd, buffer, 255, 0);
		// 	std::cout << "Client2 : " << buffer;
		// }
		// if (fds[3].revents & POLLIN) {
		// 	puts("Noice !");
		// }
	}


}

