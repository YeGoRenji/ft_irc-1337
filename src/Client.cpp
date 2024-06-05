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
	cout << "Got " << passLine;

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
		cerr << "Wrong Password" << endl;
		// TODO THROW exception (WRONG PASSWORD)
	}
}

void Client::login(Server &server) {
	this->authenticate(server);
	this->setNick();
	this->setUsernameAndRealName();

	cout << "nickname: " << this->nickname << endl;
	cout << "username: " << this->username << endl;
	cout << "realname: " << this->realname << endl;
}

void Client::setNick() {
	stringstream ss;
	getLineStream(ss);
	string token;

	// Skip NICK keyword
	ss >> token;
	if (token != "NICK") {
		// TODO : send back an error response
		cerr << "UNEXPECTED LINE !" << endl;
	}

	// Get Nick value
	ss >> token;
	this->nickname = token;

	cout << "set nickname " << nickname << endl;
}

void Client::setUsernameAndRealName() {
	stringstream ss;
	getLineStream(ss);
	string usernameToken;
	string realNameToken;

	if (!Utility::match(ss, "USER"))
	{
		// TODO : send an error
		;
	}
	ss >> usernameToken;
	if (usernameToken.empty())
	{
		// TODO : send an error
		;
	}
	if (!Utility::match(ss, "0") || !Utility::match(ss, "*"))
	{
		// TODO : send an error
		;
	}

	getline(ss, realNameToken);
	cout << "realNameToken <" << realNameToken << ">" << endl;
	realNameToken.erase(0, 1);
	if (realNameToken[0] == ':')
		realNameToken.erase(0, 1);


	// TODO : realname minlen must be 1
	if (realNameToken.empty()) {
		// TODO : send an error
		;
	}
	this->realname = realNameToken;
	this->username = usernameToken;
}

void Client::disconnect() {
	close(fdObject.getValue());
	fdObject.setValue(-1);
}

