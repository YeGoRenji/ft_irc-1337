#include<Errors.hpp>

void Errors::ERR_PASSWDMISMATCH(Client &client, Server &server)
{
	FD fd = client.getFdObject();

	static_cast<void>(client);

	string error = "464 " + server.getServerName() + " :Password incorrect\r\n";
	fd << error;
}
