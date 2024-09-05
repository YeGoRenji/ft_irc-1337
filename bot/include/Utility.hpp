#ifndef Utility_HPP
# define Utility_HPP
# include "Hacks.hpp"

class Client;
class Server;

class Utility {
public:
	static bool match(stringstream &ss, string target);
	static vector<string> splitTokensByChar(string tokens, char c);
	static vector<string> getCommandTokens(string);

private:
	Utility();
	Utility(const Utility &);
	Utility &operator=(const Utility &);
	~Utility();
};

template <typename T>
void printVector(vector<T> vec)
{
	cerr << "[";
	size_t len = vec.size();

	for (size_t i = 0; i < len; ++i) {
		cerr << vec[i];
		if (i != len - 1)
			cerr << ", ";
	}
	cerr << "]" << endl;
}

#endif
