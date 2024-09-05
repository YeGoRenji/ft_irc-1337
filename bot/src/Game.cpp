#include <Game.hpp>

Game::Game(XOBot &xoBot): xoBot(xoBot)
{
	started = false;
	whosTurn = 'X';
	memset(board, ' ', 3 * 3);

}

Game &Game::operator=(const Game &obj)
{

	if (this != &obj)
	{
		this->xoBot = obj.xoBot;
		this->started = obj.started;
		this->ONick = obj.ONick;
		this->XNick = obj.XNick;
		this->whosTurn = obj.whosTurn;
		this->winner = obj.winner;
		this->players = obj.players;
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				board[j * 3 + i] = obj.board[j * 3 + i];
	}
	return (*this);
}

Game::Game(Game const &obj): xoBot(obj.xoBot)
{

	if (this != &obj)
		*this = obj;
}

Game::~Game()
{

}

void Game::start()
{
	srand(time(NULL));
	if (rand() % 2 == 0) {
		XNick = players[0];
		ONick = players[1];
	} else {
		XNick = players[1];
		ONick = players[0];
	}

	broadCast("Match started !");
	string versus = XNick + string(" vs ") + ONick;
	broadCast(versus);
	xoBot.sendMsgToNick(XNick, "You are X!");
	xoBot.sendMsgToNick(ONick, "You are O!");

	broadCastBoard();
	printTurn(XNick);
	printTurn(ONick);

	started = true;
}

bool Game::isStarted() {
	return started;
}

bool Game::full() {
	return players.size() == 2;
}

void Game::broadCast(string msg)
{
	xoBot.sendMsgToNick(XNick, msg);
	xoBot.sendMsgToNick(ONick, msg);
}

void Game::broadCastBoard()
{
	sendBoard(XNick);
	sendBoard(ONick);
}

void Game::sendBoard(string nick)
{
	vector<string> lines(7);
	string colIndex;

	colIndex = "┌───1───2───3─┐";
	lines[0] = "│ ╔═══╦═══╦═══╗";
	lines[1] = "1 ║   ║   ║   ║";
	lines[2] = "│ ╠═══╬═══╬═══╣";
	lines[3] = "2 ║   ║   ║   ║";
	lines[4] = "│ ╠═══╬═══╬═══╣";
	lines[5] = "3 ║   ║   ║   ║";
	lines[6] = "└ ╚═══╩═══╩═══╝";

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			lines[2 * j + 1][2 + i * 6 + 4] = board[j * 3 + i];

	xoBot.sendMsgToNick(nick, colIndex);
	for (int i = 0; i < 7; ++i)
		xoBot.sendMsgToNick(nick, lines[i]);
}

void Game::addPlayer(string nick) {
	players.push_back(nick);
}

void Game::sendInfo(string nick, bool showTurn) {
	string versus = XNick + string(" vs ") + ONick;
	xoBot.sendMsgToNick(nick, versus);
	bool isX = nick == XNick;

	if (isX)
		xoBot.sendMsgToNick(nick, "You are X!");
	else
		xoBot.sendMsgToNick(nick, "You are O!");

	sendBoard(nick);

	if (showTurn)
		printTurn(isX ? XNick : ONick);
}

void Game::printTurn(string nick) {
	if ((whosTurn == 'X' && nick == XNick)
	  ||(whosTurn == 'O' && nick == ONick)) {
		xoBot.sendMsgToNick(nick, "Your turn");
		return ;
	}

	string turnStr = string(whosTurn == 'X' ? "X" : "O") + " turn";
	xoBot.sendMsgToNick(nick, turnStr);
}

bool Game::playMove(string nick, int line, int col)
{
	char who = (nick == XNick) ? 'X' : 'O';

	if (whosTurn != who)
	{
		xoBot.sendMsgToNick(nick, "Not your turn buddy :)");
		return false;
	}

	if (line > 3 || line < 1 || col > 3 || col < 1)
	{
		xoBot.sendMsgToNick(nick, "Unexpected line or column");
		return false;
	}

	int lineIdx = line - 1;
	int colIdx = col - 1;

	if (board[lineIdx * 3 + colIdx] != ' ')
	{
		xoBot.sendMsgToNick(nick, "Cell already filled up");
		return false;
	}

	board[lineIdx * 3 + colIdx] = who;

	whosTurn = whosTurn == 'O' ? 'X' : 'O';

	State state = checkState();



	sendInfo(nick, state == PROGRESS);
	sendInfo(nick == XNick ? ONick : XNick, state == PROGRESS);

	if (state != PROGRESS) {
		if (state == TIE)
			broadCast("Tie !");
		if (state == O_WIN || state == X_WIN)
		{
			broadCast(string("Winner is ") + nick + string(" (") + who + string(")"));
			winner = nick;
		}
		return true;
	}


	return false;
}

Game::State Game::checkState()
{
	for (int i = 0; i < 3; ++i) {
		if (board[i * 3 + 0] != ' '
		 && board[i * 3 + 0] == board[i * 3 + 1]
		 && board[i * 3 + 1] == board[i * 3 + 2])
			return board[i * 3] == 'X' ? X_WIN : O_WIN;

		if (board[0 * 3 + i] != ' '
		 && board[0 * 3 + i] == board[1 * 3 + i]
		 && board[1 * 3 + i] == board[2 * 3 + i])
			return board[0 * 3 + i] == 'X' ? X_WIN : O_WIN;
	}

	if (board[0 * 3 + 0] != ' '
	 && board[0 * 3 + 0] == board[1 * 3 + 1]
	 && board[1 * 3 + 1] == board[2 * 3 + 2])
		return board[1 * 3 + 1] == 'X' ? X_WIN : O_WIN;

	if (board[0 * 3 + 2] != ' '
	 && board[0 * 3 + 2] == board[1 * 3 + 1]
	 && board[1 * 3 + 1] == board[2 * 3 + 0])
		return board[1 * 3 + 1] == 'X' ? X_WIN : O_WIN;

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (board[i * 3 + j] == ' ')
				return PROGRESS;


	return TIE;
}

vector<string> &Game::getPlayers()
{
	return players;
}

bool Game::operator==(const Game &rhs)
{
	return (ONick == rhs.ONick && XNick == rhs.XNick);
}

string Game::getWinner() const
{
	return winner;
}
