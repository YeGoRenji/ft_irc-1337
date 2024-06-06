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
	int getFd();
	string getNick();
	FD &getFdObject();
	void disconnect();
	bool login(Server &server);
	
private:
	Client();
	FD fdObject;
	bool isAuthed;
	string nickname;
	string realname;
	string username;

	void setNick(Server &server);
	void authenticate(Server &server);
	void setUsernameAndRealName();
	void getLineStream(stringstream &ss);
	bool nickNameAlreadyExists(Server &server, string nickname);
};

#endif
