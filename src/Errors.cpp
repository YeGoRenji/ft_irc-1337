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

#define X(code, MACRO, message, param) \
	void Errors::ERR_##MACRO(string param, Client &client, Server &server) { \
		FD fd = client.getFdObject(); \
		\
		string error = #code; \
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


	void Errors::CUSTOM_CLIENT_GONE_TO_EDGE(Client &client)
	{

		FD fd = client.getFdObject();

		string error = "ERROR :Client gone to edge";
		error += "\r\n";
		fd << error; \
	}
#undef X
