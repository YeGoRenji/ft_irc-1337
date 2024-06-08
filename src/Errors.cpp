#include<Errors.hpp>

#define X(code, MACRO, message) \
	void Errors::ERR_##MACRO(Client &client, Server &server) { \
		FD fd = client.getFdObject(); \
		\
		string error = #code; \
		error += " "; \
		error += Utility::getClientName(client, server); \
		error += " :"; \
		error += message; \
		error += "\r\n"; \
		fd << error; \
	}
	SIMPLE_ERRORS
#undef X

/*
void Errors::ERR_CUSTOM_NOT_AUTHED(Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string error = "69 ";
	error += Utility::getClientName(client, server);
	error += " :Client is not authenticated";
	error += "\r\n";
	fd << error;

}

void Errors::ERR_NONICKNAMEGIVEN(Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string error = "431 ";
	error += Utility::getClientName(client, server);
	error += " :No nickname given";
	error += "\r\n";

	fd << error;
}

void Errors::ERR_PASSWDMISMATCH(Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string error = "464 ";
	error += Utility::getClientName(client, server);
	error += " :Password incorrect";
	error += "\r\n";

	fd << error;
}

void Errors::ERR_ALREADYREGISTERED(Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string error = "462 ";
	error += Utility::getClientName(client, server);
	error += " :You may not reregister";
	error += "\r\n";

	fd << error;
}

*/

void Errors::ERR_NEEDMOREPARAMS(string command, Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string error = "461 ";
	error += command + " ";
	error += Utility::getClientName(client, server);
	error += " :Not enough parameters";
	error += "\r\n";

	fd << error;
}

