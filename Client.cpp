#include "Client.hpp"

Client::Client(): isAuthed(false)
{
	cout << "Client: Default constructor called" << endl;
}

Client::Client(int _fd): fdObject(_fd), isAuthed(false)
{
	cout << "Client: Parameter constructor called" << endl;
}

Client::~Client()
{
	// TODO: Remove this line it was added to fix Wextra
	cout << isAuthed << endl;
	cout << "Client: Destructor called" << endl;
}

int Client::getFd() {
	return fdObject.getValue();
}

FD &Client::getFdObject() {
	return fdObject;
}
