#ifndef Bot_HPP
# define Bot_HPP

# include "FD.hpp"
# include "Hacks.hpp"
# include "Utility.hpp"

// for linux
# ifndef WAIT_ANY
#  define WAIT_ANY -1
# endif

class Bot {
public:
	Bot();
	Bot(const Bot &);
	Bot &operator=(const Bot &);
	void start();
	~Bot();
	void connectToServer(string ip, string port, string pass, string botNick);
	virtual void requestsLoop(vector<string> &tokens) = 0;
	FD &getSocket();

	
private:
	FD botSocket;
	
};

#endif
