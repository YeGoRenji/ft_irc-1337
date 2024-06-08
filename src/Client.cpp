#include <Client.hpp>
#include <Errors.hpp>

Client::Client(): isAuthed(false)
{
	// cout << "Client: Default constructor called" << endl;
}

Client::Client(int _fd): fdObject(_fd), isAuthed(false)
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

	if (tokens[1].empty())
	{
		Errors::ERR_NEEDMOREPARAMS(tokens[0], *this, server);
		return;
		//throw runtime_error("no nickname was given");
	}

	if (tokens[0][0] == '#' || tokens[0][0] == ':' || tokens[0][0] == ' ')
	{
		throw runtime_error("nickname starts with nigger listed characters");
		// TODO : send an error // dissalowed characters
	}

	if (nickNameAlreadyExists(server, tokens[1]))
	{
		throw runtime_error("nickname already exists!");
		// TODO : send an error // nickname already exists
	}

	this->nickname = tokens[1];
}

// format : <username> 0 * [:]<realname>
void Client::setUsernameAndRealName() {
	stringstream ss;
	getLineStream(ss);
	string usernameToken;
	string realNameToken;

	if (!Utility::match(ss, "USER"))
	{
		// TODO : send an error
		throw runtime_error("USER command not found");
	}

	ss >> usernameToken;
	if (usernameToken.empty())
	{
		throw runtime_error("empty username given");
		// TODO : send an error
	}

	if (!Utility::match(ss, "0") || !Utility::match(ss, "*"))
	{
		// TODO : send an error
		throw runtime_error("0 or * weren't found");
	}

	getline(ss, realNameToken);
	realNameToken.erase(0, 1);
	if (realNameToken[0] == ':')
		realNameToken.erase(0, 1);

	// TODO : realname minlen must be 1
	if (realNameToken.empty()) {
		// TODO : send an error
		throw runtime_error("realname token empty");
	}

	this->realname = realNameToken;
	this->username = usernameToken;
}

void Client::disconnect() {
	close(fdObject.getValue());
	fdObject.setValue(-1);
}
