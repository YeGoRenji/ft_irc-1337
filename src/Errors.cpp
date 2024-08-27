#include<Errors.hpp>

#define X(code, MACRO, message) \
	void Errors::ERR_##MACRO(Client &client, Server &server) { \
		string error = ":"; \
		error += server.getServerName(); \
		error += " "; \
		error += #code; \
		error += " "; \
		error += Utility::getClientName(client, server); \
		error += " :"; \
		error += message; \
		\
		client << error; \
	}
	SIMPLE_ERRORS
#undef X

#define X(code, MACRO, message, param) \
	void Errors::ERR_##MACRO(string param, Client &client, Server &server) { \
		string error = ":"; \
		error += server.getServerName(); \
		error += " "; \
		error += #code; \
		error += " "; \
		error += Utility::getClientName(client, server); \
		error += " "; \
		error += param; \
		error += " :"; \
		error += message; \
		\
		client << error; \
	}
	ONE_ARG_ERRORS

#undef X

#define X(code, MACRO, message, param, param2) \
	void Errors::ERR_##MACRO(string param, string param2, Client &client, Server &server) { \
		FD fd = client.getFdObject(); \
		\
		string error = ":"; \
		error += server.getServerName(); \
		error += " "; \
		error += #code; \
		error += " "; \
		error += Utility::getClientName(client, server); \
		error += " "; \
		error += param; \
		error += " "; \
		error += param2; \
		error += " :"; \
		error += message; \
		\
		fd << error; \
	}
	TWO_ARG_ERRORS

#undef X

void Errors::CUSTOM_CLIENT_GONE_TO_EDGE(Client &client)
{
	string error = "ERROR :Client gone to edge";
	client << error; \
}

void Errors::CUSTOM_SERVER_FULL(Client &client)
{
	string error = "ERROR :Server is Full lilassaf :'(";
	client << error; \
}

void Errors::CUSTOM_TOO_MANY_CHANNELS(Client &client)
{
	string error = "ERROR :Too many channels in server lilassaf :'(";
	client << error; \
}

void Errors::CUSTOM_INVALID_LIMIT(Client &client)
{
	string error = "ERROR :Limit should be digits only";
	client << error; \
}

void Errors::ERR_NORECIPIENT(string command, Client &client, Server &server)
{
	string error = ":"; \
	error += server.getServerName(); \
	error += " 411 "; \
	error += Utility::getClientName(client, server); \
	error += " "; \
	error += ":No recipient given (";
	error += command;
	error += ")";

	client << error;
}
