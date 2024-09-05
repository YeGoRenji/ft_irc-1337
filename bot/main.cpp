#include "XOBot.hpp"

void	check_port(string port)
{
	for (size_t i = 0; i < port.size(); i++)
		if (!isdigit(port[i]))
			throw std::runtime_error("Error: invalid port");
	int portVal = atoi(port.c_str());
	if (portVal < 0 || portVal > 65535)
		throw std::runtime_error("Error: port must be between 0 and 65535");
}

int main (int argc, char **argv) {

	if (argc != 5) {
		cerr << "Usage: ./ircBot <ip> <port> <pass> <botNickname>" << endl;
		return 1;
	}

	XOBot bot;
	string ip = argv[1];
	string port = argv[2];
	string pass = argv[3];
	string botNickname = argv[4];

	check_port(port);

	bot.connectToServer(ip, port, pass, botNickname);
	bot.start();

	return 0;
}
