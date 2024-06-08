#ifndef Client_HPP
# define Client_HPP
# include "FD.hpp"
# include "Hacks.hpp"
# include <unistd.h>
# include "Server.hpp"
# include "Utility.hpp"
# include <Replies.hpp>

class Server;

class Client {
public:
	Client(int fd);
	~Client();
	void disconnect();
	//void login(Server &server);
	void setNick(Server &server, vector<string> tokens);
	void passHandler(Server &server, vector<string> tokens);
	void setUsernameAndRealName();
	// getters;
	bool isPassGiven();
	int getFd();
	string getNick();
	FD &getFdObject();

private:
	Client();
	FD fdObject;
	// authentication thingies
	bool isAuthed;
	bool passGiven;
	bool nickGiven; // TODO : revise this function
	bool userGiven; // TODO : revise this function
	// user data
	string nickname;
	string realname;
	string username;

	void getLineStream(stringstream &ss);
	bool nickNameAlreadyExists(Server &server, string nickname);
};

#endif
