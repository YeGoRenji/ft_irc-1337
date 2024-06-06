#include "Server.hpp"

int main (int argc, char **argv) {

	if (argc != 3) {
		cerr << "Usage: ./ircserv <port> <password>" << endl;
	}

	// TODO check for argv validation

	Server srv(6969, argv[2]);

	srv.start();

	return 0;
}
