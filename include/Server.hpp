#ifndef Server_HPP
# define Server_HPP

# include <Channel.hpp>
# include <Client.hpp>
# include <Hacks.hpp>
# include <Errors.hpp>
# include <cstdio>
# include <cstring>
# include <vector>
# include <cstdlib>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <netinet/in.h>

class Client;
class Channel;

class Server {
public:
	Server(int port, string pass);
	~Server();
	void start();
	bool checkPassword(string passLine);
	bool checkUserExistence(string NickName);

	string& getServerName();

private:
	static string serverName;
	Server();
	vector<Client> clients;
	map<string, Channel> channels;
	FD serverSocket;
	string password;
	void commandsLoop(Client &currentCLient, vector<string> &tokens);

};

#endif
