#include <Client.hpp>

Client::Client():
fdObject(-1), isAuthed(false), passGiven(false), nickGiven(false), userGiven(false)
{
	cout << "Client: Default constructor called" << endl;
}

// TODO: ip should be dynamic not always "localhost"
Client::Client(int _fd):
fdObject(_fd), isAuthed(false), passGiven(false), nickGiven(false), userGiven(false), ip("localhost")
{
	cout << "Client: Parameter constructor called" << endl;

	sockaddr_in client_info;
	bzero(&client_info, sizeof(client_info));
	socklen_t info_size = sizeof(client_info);

	if (!getpeername(_fd, (sockaddr *)&client_info, &info_size)) {

		char ip_cstr[INET_ADDRSTRLEN] = "unknown";
		// TODO: can inet_ntop fail ??? what if ipv6 ? should I care ?
		inet_ntop(AF_INET, &client_info.sin_addr, ip_cstr, sizeof(ip_cstr));

		// cout << "Got ip = " << ip_cstr << endl;
		//
		ip = ip_cstr;
	}

	// TODO: should I do something if getpeername fails ?
}

Client::~Client()
{
	// TODO: Remove this line it was added to fix Wextra
	cout << "Client: Destructor called" << endl;
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

	//cout << "Got : " << passLine << endl;

	ss.str(passLine);
}

// format : PASS pass
void Client::passHandler(Server &server, vector<string> tokens) {

	if (this -> passGiven)
	{
		Errors::ERR_ALREADYREGISTERED(*this, server);
		return;
	}

	if (tokens.size() == 1)
	{
		Errors::ERR_NEEDMOREPARAMS(tokens[0], *this, server);
		return;
	}

	if (!server.checkPassword(tokens[1])) {
		Errors::ERR_PASSWDMISMATCH(*this, server);
		return;
	}

	passGiven = true;
	isAuthed = passGiven & nickGiven & userGiven;
	cout << isPassGiven() << endl;
}

string &Client::getNick()
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
// TODO : check the clients can change their nick!!!

// format : NICKNAME nick
void Client::handleNICK(Server &server, vector<string> tokens) {

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

	setNick(server, tokens[1]);

	nickGiven = true;
	if (!isAuthed && (passGiven & nickGiven & userGiven))
		this -> beWelcomed(server);
	isAuthed = passGiven & nickGiven & userGiven;
}

void Client::beWelcomed(Server &server)
{
	Replies::RPL_WELCOME(*this, server);
	Replies::RPL_YOURHOST(*this, server);
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

	this->username = tokens[1];
	this->realname = tokens[4];

	userGiven = true;
	if (!isAuthed && (passGiven & nickGiven & userGiven))
		this -> beWelcomed(server);
	isAuthed = passGiven & nickGiven & userGiven;
}

Client &Client::operator<<(std::string str) {
	fdObject << str;
	return (*this);
}

void Client::operator>>(std::string& str) {
	string buffer;
	fdObject >> buffer;
	command += buffer;

	cerr << "\nBuffered <" << buffer << "> from Client " << fdObject.getValue() << " (" << this -> nickname << ") " << this << endl;
	if (!CONTAINS(command, "\r\n"))
		return;

	str = string(command.begin(), command.end() - 2);
	cerr << "Full command : <" << str << ">" << endl;
	command.clear();
}

void Client::disconnect() {
	close(fdObject.getValue());
	fdObject.setValue(-1);
}

void Client::leaveAllChannels(Server &server, string reason)
{
	map<string, Channel> &channels = server.getChannels();


	vector<Channel *> joinedChannels;
	for (map<string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->second.hasMember(nickname))
			joinedChannels.push_back(&it->second);
	}

	for (size_t i = 0; i < joinedChannels.size(); ++i)
		server.RemoveMemberFromChannel(*joinedChannels[i], *this, reason);
}

// getters
string &Client::getIp()
{
	return (this->ip);
}

string &Client::getUsername()
{
	return (this->username);
}

// setters (a7san wa7d)
void Client::setNick(Server &server, string &nick)
{
	vector<Channel *> clientChannels = server.getChannelsWithMember(this->nickname);
	vector<Channel *> channelsWhereWasOp;

	for (size_t i = 0; i < clientChannels.size(); ++i)
	{
		if ((*clientChannels[i]).isOperator(this->nickname))
			channelsWhereWasOp.push_back(clientChannels[i]);
		(*clientChannels[i]).removeMemberSilently(*this);
	}

	this->nickname = nick;

	for (size_t i = 0; i < clientChannels.size(); ++i)
		(*clientChannels[i]).addMemberSilently(*this);

	for (size_t i = 0; i < channelsWhereWasOp.size(); ++i)
		(*channelsWhereWasOp[i]).addOperator(*this);
}
