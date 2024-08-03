#include "Server.hpp"

void	check_port(string port)
{
	for (size_t i = 0; i < port.size(); i++)
		if (!isdigit(port[i]))
			throw std::runtime_error("\e[033;0;31m	⚡︎   ❌ Error PORT ❌	⚡\e[0m");
}

int main (int argc, char **argv) {

	if (argc != 3) {
		cerr << "Usage: ./ircserv <port> <password>" << endl;
		return 1;
	}

	try {

		string	port = argv[1];
		string	password = argv[2];

		check_port(port);

		Server	srv(atoi(port.c_str()), password);

		srv.start();

	} catch (std::exception &e) {
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}
