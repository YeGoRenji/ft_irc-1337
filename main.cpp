#include "Server.hpp"

void	check_port(string port)
{
	for (size_t i = 0; i < port.size(); i++)
	{
		if (!isdigit(port[i]))
		{
			cerr << "\e[033;0;31m	⚡︎   ❌ Error PORT ❌	⚡\e[0m" << endl;
			exit(1);
		}
	}
}

int main (int argc, char **argv) {

	if (argc == 3)
	{
		string	port = argv[1];

		check_port(port);

		Server	srv(atoi(argv[1]), argv[2]);

		srv.start();

	}
	else
	{
		cerr << "Usage: ./ircserv <port> <password>" << endl;
		return 1;
	}

	return 0;
}
