#include <Client.hpp>
#include <Errors.hpp>

Client::Client(): isAuthed(false)
{
	// cout << "Client: Default constructor called" << endl;
}

Client::Client(int _fd):
fdObject(_fd), isAuthed(false), passGiven(false), nickGiven(false), userGiven(false)
{
	// cout << "Client: Parameter constructor called" << endl;
}

Client::~Client()
{
	// TODO: Remove this line it was added to fix Wextra
	// cout << "Client: Destructor called" << endl;
}

int Client::getFd() {
	return fdObject.getValue();
}

FD &Client::getFdObject() {
	return fdObject;
}

void Client::getLineStream(stringstream &ss) {
	string passLine;
	fdObject >> passLine;

	cout << "Got : " << passLine << endl;

	ss.str(passLine);
}

// format : PASS pass
void Client::passHandler(Server &server, vector<string> tokens) {

	if (this -> passGiven)
	{
		Errors::ERR_ALREADYREGISTERED(*this, server);
		return;
		//throw runtime_error("ERR_ALREADYREGISTERED");
	}

	if (tokens.size() == 1)
	{
		Errors::ERR_NEEDMOREPARAMS(tokens[0], *this, server);
		return;
		//throw runtime_error("ERR_NEEDMOREPARAMS");
	}

	if (!server.checkPassword(tokens[1])) {
		Errors::ERR_PASSWDMISMATCH(*this, server);
		return;
		//throw runtime_error("Wrong Password");
	}

	passGiven = true;
	isAuthed = passGiven & nickGiven & userGiven;
	cout << "goooood" << endl;
	cout << isPassGiven() << endl;
}

/*
void Client::login(Server &server) 
{
	try {
		if (this -> passGiven == false)
			this->authenticate(server);
		this->setNick(server);
		this->setUsernameAndRealName();
		cout << "nickname: " << this->nickname << endl;
		cout << "username: " << this->username << endl;
		cout << "realname: " << this->realname << endl;

		Replies::RPL_WELCOME(*this, server);
		this -> isAuthed = true;
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
	}
}
*/

string Client::getNick()
{
	return (this -> nickname);
}

bool Client::isPassGiven()
{
	return (this -> passGiven);
}

bool Client::nickNameAlreadyExists(Server &server, string nickname)
{
	return server.checkUserExistence(nickname);
}

// The NICK message may be sent from the server to clients to acknowledge their NICK command was successful, and to inform other clients about the change of nickname. In these cases, the <source> of the message will be the old nickname [ [ "!" user ] "@" host ] of the user who is changing their nickname.
// TODO : maybe do this ? uwu? maybe not?


// format : NICKNAME nick
void Client::setNick(Server &server, vector<string> tokens) {

	if (tokens.size() == 1)
	{
		Errors::ERR_NEEDMOREPARAMS(tokens[0], *this, server);
		return;
	}

	if (tokens[1].empty())
	{
		Errors::ERR_ERRONEUSNICKNAME(tokens[0], *this, server);
		return;
		//throw runtime_error("no nickname was given");
	}

	if (tokens[1][0] == '#' || tokens[1][0] == ':' || tokens[1][0] == ' ')
	{
		Errors::ERR_ERRONEUSNICKNAME(tokens[1], *this, server);
		return;
		//throw runtime_error("nickname starts with nigger listed characters");
	}

	if (nickNameAlreadyExists(server, tokens[1]))
	{
		Errors::ERR_NICKNAMEINUSE(tokens[1], *this, server);
		return ;
		//throw runtime_error("nickname already exists!");
	}

	this->nickname = tokens[1];
	nickGiven = true;
	if (!isAuthed && (passGiven & nickGiven & userGiven))
		Replies::RPL_WELCOME(*this, server);
	isAuthed = passGiven & nickGiven & userGiven;
}

// TODO : what the fuck is an ident server in the rfc??
// format : USER <username> 0 * [:]<realname>
void Client::setUsernameAndRealName(Server &server, vector<string> tokens)
{
	if (userGiven)
	{
		Errors::ERR_ALREADYREGISTERED(*this, server);
		return;
	}

	if (tokens.size() < 5)
	{
		Errors::ERR_NEEDMOREPARAMS(tokens[0], *this, server);
		return;
	}

	// TODO : what to do here??
	/*
	if (!Utility::match(ss, "0") || !Utility::match(ss, "*"))
	{
		// TODO : send an error
		throw runtime_error("0 or * weren't found");
	}
	*/

	this->username = tokens[1];
	this->realname = tokens[4];

	userGiven = true;
	if (!isAuthed && (passGiven & nickGiven & userGiven))
		Replies::RPL_WELCOME(*this, server);
	isAuthed = passGiven & nickGiven & userGiven;
}

void Client::disconnect() {
	close(fdObject.getValue());
	fdObject.setValue(-1);
}
