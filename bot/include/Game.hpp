#ifndef Game_HPP
# define Game_HPP
# include "Hacks.hpp"
# include "XOBot.hpp"
# include <ctime>
# include <cstring>

class XOBot;

class Game {
public:
	Game(XOBot &);
	Game(const Game &);
	Game &operator=(const Game &);
	~Game();
	void start();
	void addPlayer(string nick);
	bool isStarted();
	bool full();
	void broadCast(string msg);
	void broadCastBoard();
	void printTurn(string nick);
	void sendBoard(string nick);
	void sendInfo(string nick, bool showTurn);
	bool playMove(string nick, int line, int col);
	vector<string> &getPlayers();

	enum State {
		PROGRESS,
		TIE,
		X_WIN,
		O_WIN
	};

	State checkState();
	string getWinner(void) const;

	bool operator==(const Game &rhs);
	

private:
	XOBot &xoBot;
	char board[3 * 3];
	string XNick;
	string ONick;
	vector<string> players;
	bool started;
	char whosTurn;
	string winner;

	char getBoardChar();
};

#endif
