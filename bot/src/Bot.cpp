#include <Bot.hpp>

int chk(int status, const std::string msg) {
	if (status < 0) {
		std::cerr << "Error[" << status << "]\t" << msg << ", Reason: " << strerror(errno) << endl;
		exit(status);
	}
	return status;
}

Bot::Bot()
{

}

Bot &Bot::operator=(const Bot &obj)
{

	if (this != &obj)
	{
		this->botSocket = obj.botSocket;
	}
	return (*this);
}

Bot::Bot(Bot const &obj)
{

	if (this != &obj)
		*this = obj;
}

Bot::~Bot()
{

}

void Bot::connectToServer(string ip, string port, string pass, string botNick)
{
	botSocket.setValue(chk(socket(AF_INET, SOCK_STREAM, 0), "Couldn't open socket"));



	addrinfo hints, *res = NULL;
	bzero(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	chk(getaddrinfo(ip.c_str(), port.c_str(), &hints, &res), "Couldn't resolve address");
	if (res == NULL)
		chk(-1, "Couldn't resolve address");

	chk(connect(botSocket.getValue(), res->ai_addr, res->ai_addrlen), "Couldn't connect");

	freeaddrinfo(res);

	fcntl(botSocket.getValue(), F_SETFL, O_NONBLOCK);

	cerr << "connected to " << ip.c_str() << endl;


	string reply;
	botSocket << (string("PASS ") + pass);
	botSocket << (string("NICK ") + botNick);
	botSocket << "USER bot blip blop bot";
}

void Bot::start() {

	pollfd fd = { botSocket.getValue(), POLLIN, 0 };

	while(69) {

		chk(poll(&fd,  1, WAIT_ANY), "poll failed");

		if (fd.revents & POLLHUP) {
			cerr << "Server closed connection" << endl;
			close(botSocket.getValue());
			return;
		}
		else if (fd.revents & POLLIN) {
			string data;

			botSocket >> data;
			if (data.empty()) {
				close(botSocket.getValue());
				return;
			}
			vector<string> tokens = Utility::getCommandTokens(data);
			fd.revents = 0;
			if (tokens.size())
				requestsLoop(tokens);
		}
	}
}


FD &Bot::getSocket(void) {
	return this->botSocket;
}
