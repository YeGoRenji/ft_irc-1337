#include<Errors.hpp>

#define X(code, MACRO, message) \
	void Errors::ERR_##MACRO(Client &client, Server &server) { \
		FD fd = client.getFdObject(); \
		\
		string error = ":"; \
		error += server.getServerName(); \
		error += " "; \
		error += #code; \
		error += " "; \
		error += Utility::getClientName(client, server); \
		error += " :"; \
		error += message; \
		error += "\r\n"; \
		fd << error; \
	}
	SIMPLE_ERRORS
#undef X

#define X(code, MACRO, message, param) \
	void Errors::ERR_##MACRO(string param, Client &client, Server &server) { \
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
		error += " :"; \
		error += message; \
		error += "\r\n"; \
		fd << error; \
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
		error += "\r\n"; \
		fd << error; \
	}
	TWO_ARG_ERRORS

#undef X

void Errors::CUSTOM_CLIENT_GONE_TO_EDGE(Client &client)
{
	FD fd = client.getFdObject();

	string error = "ERROR :Client gone to edge";
	error += "\r\n";
	fd << error; \
}
