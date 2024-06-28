#ifndef Server_HPP
# define Server_HPP

# include <Channel.hpp>
# include <Client.hpp>
# include <Hacks.hpp>
# include <Errors.hpp>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <netinet/in.h>

// for linux
# ifndef WAIT_ANY
#  define WAIT_ANY -1
# endif

class Client;
class Channel;

struct channelInfo {
	string name;
	string password;
};

class Server {
public:
	Server(int port, string pass);
	~Server();
	void start();
	bool checkPassword(string passLine);
	bool checkUserExistence(string nickName);
	void parseChannelCommand(vector<channelInfo> &ch, string channelsTokens, string passswordsTokens);
	void RemoveMemberFromChannel(Channel &channel, Client &client, string reason);
	static string serverName;

	// getter
	string& getServerName();
	map<int, Client>::iterator getClientFromNick(string &nick);
	map<string, Channel> &getChannels();

private:
	void commandsLoop(Client &currentCLient, vector<string> &tokens, vector<pollfd> &fds);
	void quitUser(Client &currClient, vector<pollfd> &fds, string reason);
	void AddClientoChannel(Client &client, vector<string> &tokens);
	map<string, Channel>::iterator getChannel(string name);
	map<string, Channel>::iterator createChannel(string name, string password);
	void handlePART(Client &client, vector<string> &tokens);
	void KickClientFromChannel(Client &client, vector<string> &tokens);
	void TopicClientFromChannel(Client &client, vector<string> &tokens);
	void InviteClientFromChannel(Client &client, vector<string> &tokens);

	Server();
	map<int, Client> clients;
	map<string, Channel> channels;
	FD serverSocket;
	string password;
};
#endif
