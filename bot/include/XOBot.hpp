#ifndef XOBot_HPP
# define XOBot_HPP
# include "Hacks.hpp"
# include "Game.hpp"
# include "Bot.hpp"
# include <iomanip>

class Game;

class XOBot : public Bot {
public:
	XOBot();
	XOBot(const XOBot &);
	XOBot &operator=(const XOBot &);
	~XOBot();
	virtual void requestsLoop(vector<string> &tokens);
	void sendMsgToNick(string nick, string msg);
	void commandsLoop(string nick, vector<string> &tokens);
	Game &matchMake(string nick);
	void handlePlay(string nick);
	void handleInfo(string nick);
	void handlePut(string nick, vector<string> tokens);
	void handleLeave(string nick);
	void handleScore(string nick);
	void closeMatch(Game *game);
	
private:
	deque<Game*> matches;
	map<string, Game*> currentPlayers;
	bool isValidIndex(string str);

	map<string, unsigned long> scoreBoard;

	Game *matchStartedCheck(string nick);
};

#endif
