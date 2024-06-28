#ifndef Client_HPP
# define Client_HPP
# include <FD.hpp>
# include <Hacks.hpp>
# include <unistd.h>
# include <Server.hpp>
# include <Utility.hpp>
# include <Replies.hpp>
#include <Errors.hpp>

class Server;

class Client {
public:
	Client();
	Client(int fd);
	~Client();
	void disconnect();
	void leaveAllChannels(Server &server, string reason);

	//void login(Server &server);
	void setNick(Server &server, vector<string> tokens);
	void passHandler(Server &server, vector<string> tokens);
	void setUsernameAndRealName(Server &server, vector<string> vectors);
	void beWelcomed(Server &server);
	// getters;
	bool isPassGiven();
	bool wasWelcomed;
	int getFd();
	string &getNick();
	string &getUsername();
	FD &getFdObject();
	string &getIp();


	Client &operator<<(std::string);
	void operator>>(std::string&);

private:
	FD fdObject;
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

	void getLineStream(stringstream &ss);
	bool nickNameAlreadyExists(Server &server, string nickname);
};

#endif
