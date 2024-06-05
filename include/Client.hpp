#ifndef Client_HPP
# define Client_HPP
# include "FD.hpp"
# include "Hacks.hpp"
# include <unistd.h>
# include "Server.hpp"
# include "Utility.hpp"

class Server;

class Client {
public:
	Client(int fd);
	~Client();
	int getFd();
	FD &getFdObject();
	void disconnect();
	void login(Server &server);
	
private:
	Client();
	FD fdObject;
	bool isAuthed;
	string nickname;
	string realname;
	string username;

	void setNick();
	void authenticate(Server &server);
	void setUsernameAndRealName();
	void getLineStream(stringstream &ss);

};

#endif
