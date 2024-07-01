#include "Utility.hpp"
#include "Hacks.hpp"

bool Utility::match(stringstream &ss, string target)
{
	string keyword;
	ss >> keyword;
	return (keyword == target);
}

string &Utility::getClientName(Client &client, Server &server)
{
	if (!client.getNick().empty())
		return client.getNick();
	return server.getServerName();
}

vector<string> Utility::splitTokensByChar(string tokens, char c)
{
	replace(tokens.begin(), tokens.end(), c, ' ');
	return getCommandTokens(tokens);
}

vector<string> Utility::getCommandTokens(string command)
{
	string tmpToken;
	string tmpTmpToken;
	vector<string> tokens;
	stringstream ss(command);

	if (!command.size())
		return tokens;

	while(ss >> tmpToken)
	{
		if (tmpToken[0] == ':')
		{
			tmpToken.erase(0, 1);
			getline(ss, tmpTmpToken);
			tmpToken += tmpTmpToken;
		}
		tokens.push_back(tmpToken);
	}
	return tokens;
}

string Utility::constructMemberList(map<string, Client *> &members, const string prefix)
{
	string str;
	map<string, Client*>::iterator it = members.begin();
	map<string, Client*>::iterator ite = members.end();

	for (; it != ite; ++it)
	{
		str += prefix;
		str += it->second->getNick();
		str += " ";
	}
	return str;
}
