#ifndef Server_HPP
# define Server_HPP

# include <Channel.hpp>
# include <Client.hpp>
# include <Hacks.hpp>
# include <Errors.hpp>
# include <unistd.h>
# include <Replies.hpp>

// max users in listen(2) queue
# ifndef MAX_LISTEN
#  define MAX_LISTEN 128
# endif

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
	static string serverName;
	Server(uint16_t port, string pass);
	~Server();
	void start();
	bool checkPassword(string passLine);
	bool checkUserExistence(string nickName);
	void parseChannelsToken(vector<channelInfo> &ch, string channelsTokens, string passswordsTokens);
	void RemoveMemberFromChannel(Channel &channel, Client &client, string reason);
	bool hasMember(string &nick);
	bool hasChannel(string ChannelName);

	// getter
	string& getServerName();
	map<int, Client>::iterator getClientFromNick(string &nick);
	map<string, Channel> &getChannels();
	vector<Channel *> getChannelsWithMember(string nick);
	map<string, Channel>::iterator getChannel(string name);

	void	sendChannelModeToClient(Channel &channelObj, Client &client);
	void	applyModeToChannel(Channel &channelObj, Client &client, vector<string> &tokens);

private:
	void commandsLoop(Client &currentCLient, vector<string> &tokens, vector<pollfd> &fds);
	void quitUser(Client &currClient, vector<pollfd> &fds, string reason);
	void handleJOIN(Client &client, vector<string> &tokens);

	map<string, Channel>::iterator createChannel(string name, string password);

	void handlePART(Client &client, vector<string> &tokens);
	void handleMODE(Client &client, vector<string> &tokens);
	void handleKICK(Client &client, vector<string> &tokens);
	void handleTOPIC(Client &client, vector<string> &tokens);
	void handleINVITE(Client &client, vector<string> &tokens);
	void handlePRIVMSG(Client &sender, vector<string> &tokens);

	Server();
	map<int, Client> clients;
	map<string, Channel> channels;
	FD serverSocket;
	string password;
};
#endif
