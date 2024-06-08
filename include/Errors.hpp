#ifndef Errors_HPP
# define Errors_HPP

#include <Hacks.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <Utility.hpp>

#define SIMPLE_ERRORS \
	X(69, CUSTOM_NOT_AUTHED, "Client is not authenticated") \
	X(431, NONICKNAMEGIVEN, "No nickname given") \
	X(462, ALREADYREGISTERED, "You may not reregister") \
	X(464, PASSWDMISMATCH, "Password incorrect")

#define ONE_ARG_ERRORS \
	X(432, ERRONEUSNICKNAME, "Erroneus nickname", nick) \
	X(433, NICKNAMEINUSE, "Nickname is already in use", nick) \
	X(461, NEEDMOREPARAMS, "Not enough parameters", command)

class Server;
class Client;

class Errors {
public:
	#define X(code, MACRO, message) \
		static void ERR_##MACRO(Client &client, Server &server);
		SIMPLE_ERRORS
	#undef X

	#define X(code, MACRO, message, param) \
		static void ERR_##MACRO(string param, Client &client, Server &server);
		ONE_ARG_ERRORS
	#undef X

private:
	Errors();
	Errors(const Errors &);
	Errors &operator=(const Errors &);
	~Errors();
};

#endif
