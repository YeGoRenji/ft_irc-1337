#include <XOBot.hpp>

XOBot::XOBot()
{

}

XOBot &XOBot::operator=(const XOBot &obj)
{

	if (this != &obj)
	{
		Bot::operator=(obj);
	}
	return (*this);
}

XOBot::XOBot(XOBot const &obj): Bot(obj)
{

	if (this != &obj)
		*this = obj;
}

XOBot::~XOBot()
{

	for (size_t i = 0; i < matches.size(); ++i)
		delete matches[i];
}

void XOBot::commandsLoop(string nick, vector<string> &tokens)
{
	if (tokens.size() < 1) {
		sendMsgToNick(nick, "Got empty msg..");
		return ;
	}

	if (tokens[0] == "PLAY") {
		handlePlay(nick);
	} else if (tokens[0] == "WTF") {
		sendMsgToNick(nick, "yep no idea\nmaybe");
	} else if (tokens[0] == "INFO") {
		handleInfo(nick);
	} else if (tokens[0] == "PUT") {
		handlePut(nick, tokens);
	} else if (tokens[0] == "SCORE") {
		handleScore(nick);
	} else if (tokens[0] == "LEAVE") {
		handleLeave(nick);
	} else {
		sendMsgToNick(nick, "What are you saying mate, Try: PLAY, INFO, PUT, SCORE, LEAVE");
	}
}

void XOBot::requestsLoop(vector<string> &tokens)
{
	if (tokens.size() < 2)
		return ;

	if (tokens[1] == "PRIVMSG") {
		vector<string> botCommands = Utility::getCommandTokens(tokens[3]);
		commandsLoop(string(tokens[0].begin()+1, tokens[0].end()), botCommands);
	} else if (tokens[1] == "464") {
		cerr << "Error: Password Incorrect" << endl;
		exit(1);
	} else if (tokens[1] == "433") {
		cerr << "Error: Nickname is already in use" << endl;
		exit(1);
	}
}

void XOBot::sendMsgToNick(string nick, string msg)
{
	string reply = "PRIVMSG ";
	reply += nick;
	reply += " :";
	reply += msg;
	getSocket() << reply;
}

bool XOBot::isValidIndex(string str)
{
	if (str.size() != 1)
		return false;

	if (!isdigit(str[0]))
		return false;

	return true;
}

Game &XOBot::matchMake(string nick) {

	for (size_t i = 0; i < matches.size(); ++i) {
		if (!matches[i]->isStarted() && !matches[i]->full()) {
			matches[i]->addPlayer(nick);
			return *matches[i];
		}
	}

	Game* game = new Game(*this);
	game->addPlayer(nick);
	matches.push_back(game);

	return *matches.back();
}

void XOBot::handlePlay(string nick) {
	map<string, Game*>::iterator it = currentPlayers.find(nick);

	if (it == currentPlayers.end()) {
		Game &game = matchMake(nick);
		currentPlayers[nick] = &game;
		if (game.full()) {
			game.start();
			return ;
		}
		sendMsgToNick(nick, "Round created! Waiting for opponent...");
		return ;
	}

	Game *game = it->second;
	if (game->isStarted()) {
		sendMsgToNick(nick, "You can't play until you finish your current match");
		return ;
	}

	sendMsgToNick(nick, "Waiting for opponent...");
}

void XOBot::handleInfo(string nick)
{
	Game *game = matchStartedCheck(nick);

	if (game == NULL)
		return ;

	game->sendInfo(nick, true);
}

Game *XOBot::matchStartedCheck(string nick) {
	map<string, Game*>::iterator it = currentPlayers.find(nick);

	if (it == currentPlayers.end())
	{
		sendMsgToNick(nick, "You aren't in any match yet, try to run: PLAY");
		return NULL;
	}

	Game *game = it->second;
	if (!game->isStarted())
	{
		sendMsgToNick(nick, "Your match didn't start yet. Waiting for opponent...");
		return NULL;
	}

	return game;
}

void XOBot::handlePut(string nick, vector<string> tokens)
{
	if (tokens.size() != 3)
		return sendMsgToNick(nick, "Usage: PUT <line> <column>");

	Game *game = matchStartedCheck(nick);

	if (game == NULL)
		return ;

	string lineStr = tokens[1];
	string colStr = tokens[2];
	if (!isValidIndex(lineStr) || !isValidIndex(colStr))
		return sendMsgToNick(nick, "Unexpected line or column");

	int line = atoi(lineStr.c_str());
	int col = atoi(colStr.c_str());

	if (game->playMove(nick, line, col))
	{
		// Match ended
		if (!game->getWinner().empty())
			scoreBoard[game->getWinner()]++;
		game->broadCast("Match Ended");
		closeMatch(game);
	}
}

void XOBot::closeMatch(Game *game) {
	vector<string> &players = game->getPlayers();
	currentPlayers.erase(players[0]);
	currentPlayers.erase(players[1]);
	deque<Game*>::iterator it = find(matches.begin(), matches.end(), game);
	if (it == matches.end()) {

		exit(1);
	}

	delete *it;
	matches.erase(it);

}

void XOBot::handleLeave(string nick)
{
	Game *game = matchStartedCheck(nick);

	if (game == NULL)
		return;

	string leaveMsg = string("Game aborted cause player ") + nick + string(" Left !");
	game->broadCast(leaveMsg);

	closeMatch(game);
}

std::string	truncate(std::string str, unsigned int width)
{
	std::string new_str;

	if (str.length() <= width)
		return (str);
	new_str = str.substr(0, width - 1) + '.';
	return (new_str);
}

string	row(std::string col1, std::string col2)
{
	stringstream ss;
	ss << "║" << std::setw(10) << truncate(col1, 10);
	ss << "║" << std::setw(10) << truncate(col2, 10) << "║\n";

	return ss.str();
}

void XOBot::handleScore(string nick)
{
	if (scoreBoard.empty())
		return sendMsgToNick(nick, "No data in scoreboard");

	map<string, unsigned long>::iterator it = scoreBoard.begin();
	map<string, unsigned long>::iterator ite = scoreBoard.end();

	sendMsgToNick(nick, "╔══════════╦══════════╗");
	sendMsgToNick(nick, row("Nick", "Score"));
	sendMsgToNick(nick, "╠══════════╬══════════╣");
	for (; it != ite; ++it) {
		stringstream parseInt;
		string scoreNick = it->first;
		unsigned long scoreValue = it->second;

		parseInt << scoreValue;
		sendMsgToNick(nick, row(scoreNick, parseInt.str()));
	}
	sendMsgToNick(nick, "╚══════════╩══════════╝");
}
