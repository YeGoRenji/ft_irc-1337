#ifndef Errors_HPP
# define Errors_HPP

#include<Hacks.hpp>
#include<Client.hpp>
#include<Server.hpp>

class Server;
class Client;

class Errors {
public:
	static void ERR_PASSWDMISMATCH(Client &client, Server &server);

private:
	Errors();
	Errors(const Errors &);
	Errors &operator=(const Errors &);
	~Errors();
};

#endif
