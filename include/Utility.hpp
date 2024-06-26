#ifndef Utility_HPP
# define Utility_HPP
# include <Hacks.hpp>
# include <Client.hpp>
# include <Server.hpp>

class Client;
class Server;

class Utility {
public:
	static bool match(stringstream &ss, string target);
	static string &getClientName(Client &client, Server &Server);
	static vector<string> getCommandTokens(string);

private:
	Utility();
	Utility(const Utility &);
	Utility &operator=(const Utility &);
	~Utility();
};

#endif
