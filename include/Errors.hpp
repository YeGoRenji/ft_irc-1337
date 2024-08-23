#ifndef Errors_HPP
# define Errors_HPP

#include <Hacks.hpp>
#include <Client.hpp>
#include <Server.hpp>
#include <Utility.hpp>

#define SIMPLE_ERRORS \
	X(69, CUSTOM_NOT_AUTHED, "Client is not authenticated") \
	X(412, NOTEXTTOSEND, "No text to send") \
	X(431, NONICKNAMEGIVEN, "No nickname given") \
	X(462, ALREADYREGISTERED, "You may not reregister") \
	X(464, PASSWDMISMATCH, "Password incorrect")

#define ONE_ARG_ERRORS \
	X(401, NOSUCHNICK, "No such nick/channel", recevier) \
	X(403, NOSUCHCHANNEL, "No such channel", channel) \
	X(404, CANNOTSENDTOCHAN, "Cannot send to channel", channel) \
	X(421, UNKNOWNCOMMAND, "ach had l command dzb ???", command) \
	X(432, ERRONEUSNICKNAME, "Erroneus nickname", nick) \
	X(433, NICKNAMEINUSE, "Nickname is already in use", nick) \
	X(442, NOTONCHANNEL, "You're not on that channel", channel) \
	X(461, NEEDMOREPARAMS, "Not enough parameters", command) \
	X(475, BADCHANNELKEY, "Cannot join channel, you need channel password (+k)", channel) \
	X(471, CHANNELISFULL, "Cannot join channel, channel is full (+l)", channel) \
	X(473, INVITEONLYCHAN, "Cannot join channel, you must be invited (+i)", channel) \
	X(482, CHANOPRIVSNEEDED, "You're not channel operator", channel) \
	X(472, UNKNOWNMODE, "is unknown mode char to me", modechar)

#define TWO_ARG_ERRORS \
	X(441, USERNOTINCHANNEL, "They aren't on that channel", nick, channel) \
	X(443, USERONCHANNEL, "is already on channel", nick, channel) \
	X(467, KEYALREADYSET, "Channel key already set", nick, channel)


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

	#define X(code, MACRO, message, param, param2) \
		static void ERR_##MACRO(string param, string param2, Client &client, Server &server);
		TWO_ARG_ERRORS
	#undef X

	static void	CUSTOM_CLIENT_GONE_TO_EDGE(Client &client);
	static void	ERR_NORECIPIENT(string command, Client &client, Server &server);

private:
	Errors();
	Errors(const Errors &);
	Errors &operator=(const Errors &);
	~Errors();
};

#endif
