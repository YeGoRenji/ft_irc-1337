#include <Utility.hpp>

bool Utility::match(stringstream &ss, string target)
{
	string keyword;
	ss >> keyword;
	return (keyword == target);
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

	int iter = 0;

	if (!command.size())
		return tokens;

	while(ss >> tmpToken)
	{
		if (tmpToken[0] == ':' && iter)
		{
			tmpToken.erase(0, 1);
			getline(ss, tmpTmpToken);
			tmpToken += tmpTmpToken;
		}
		tokens.push_back(tmpToken);
		++iter;
	}
	return tokens;
}
