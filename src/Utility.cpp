#include "Utility.hpp"
#include "Hacks.hpp"

bool Utility::match(stringstream &ss, string target)
{
	string keyword;
	ss >> keyword;
	return (keyword == target);
}

string Utility::getClientName(Client &client, Server &server)
{
	if (!client.getNick().empty())
		return client.getNick();
	return server.getServerName();
}


vector<string> Utility::getCommandTokens(string command)
{
	string tmpToken;
	string tmpTmpToken;
	vector<string> tokens;
	stringstream ss(command);

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
