#ifndef Client_HPP
# define Client_HPP
# include <FD.hpp>
# include <Hacks.hpp>
# include <unistd.h>
# include <Server.hpp>
# include <Utility.hpp>
# include <Replies.hpp>
# include <Errors.hpp>

class Server;

class Client {
public:
	Client();
	Client(int fd);
	~Client();
	void disconnect();
	void leaveAllChannels(Server &server, string reason);
	string craftSourceMessage(string command, string message);

	//void login(Server &server);
	void handleNICK(Server &server, vector<string> tokens);
	void passHandler(Server &server, vector<string> tokens);
	void setUsernameAndRealName(Server &server, vector<string> vectors);
	void beWelcomed(Server &server);

	// getters;
	bool isPassGiven();
	bool isAuthenticated();
	bool wasWelcomed;
	int getFd();
	string &getNick();
	string &getUsername();
	FD &getFdObject();
	string &getIp();

	// setters;
	void setNick(Server &server, string &nick);
	Client &operator<<(std::string);
	bool operator>>(std::string&);

	// statics;
	bool isValidNick(string nick);

private:
	FD fdObject;
	string	command;
	// authentication thingies
	bool isAuthed;
	bool passGiven;
	bool nickGiven;
	bool userGiven; // TODO : revise this function
	// user data
	string nickname;
	string realname;
	string username;
	string ip;

	bool nickNameAlreadyExists(Server &server, string nickname);
};

#endif
