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
# include <cerrno>

// for linux
# ifndef WAIT_ANY
#  define WAIT_ANY -1
# endif

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
	void commandsLoop(Client &currentCLient, vector<string> &tokens, vector<pollfd> &fds);
	void quitUser(Client &client, vector<pollfd> &fds);

	static string serverName;
	Server();
	vector<Client> clients;
	map<string, Channel> channels;
	FD serverSocket;
	string password;
};
#endif
