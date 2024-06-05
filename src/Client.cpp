#include "Client.hpp"

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
	cout << isAuthed << endl;
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

void Client::authenticate(Server &server) {
	string passLine;
	fdObject >> passLine;

	// TODO : send errors in case the pass is wrong

	cout << "passLine <" << passLine << ">" << endl;

	if (server.checkPassword(passLine)) {
		cout << "Connected" << endl;
		this->isAuthed = true;
	} else {
		throw runtime_error("Wrong Password");
		// TODO THROW exception (WRONG PASSWORD)
	}
}

bool Client::login(Server &server) {
	try {
		this->authenticate(server);
		this->setNick(server);
		this->setUsernameAndRealName();
		cout << "nickname: " << this->nickname << endl;
		cout << "username: " << this->username << endl;
		cout << "realname: " << this->realname << endl;
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
		return false;
	}
	return true;
}

string Client::getNick()
{
	return (this -> nickname);
}

bool Client::nickNameAlreadyExists(Server &server, string nickname)
{
	return server.checkUserExistence(nickname);
}

void Client::setNick(Server &server) {
	stringstream ss;
	getLineStream(ss);
	string token;

	// Skip NICK keyword
	ss >> token;
	if (token != "NICK") {
		// TODO : send back an error response
		throw runtime_error("no NICK command");
	}

	// Get Nick value
	ss >> token;

	if (token.empty())
	{
		// TODO : send an error // dissalowed characters
		throw runtime_error("no nickname was given");
	}

	if (token[0] == '#' || token[0] == ':' || token[0] == ' ')
	{
		throw runtime_error("nickname starts with nigger listed characters");
		// TODO : send an error // dissalowed characters
	}

	if (nickNameAlreadyExists(server, token))
	{
		throw runtime_error("nickname already exists!");
		// TODO : send an error // nickname already exists
	}

	this->nickname = token;
	cout << "nickname : " << token << endl;
}

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
	cout << "realNameToken <" << realNameToken << ">" << endl;
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
	cout << "realname : " << realname << endl;
	cout << "username : " << username << endl;
}

void Client::disconnect() {
	close(fdObject.getValue());
	fdObject.setValue(-1);
}

