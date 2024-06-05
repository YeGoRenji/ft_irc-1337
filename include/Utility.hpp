#ifndef Utility_HPP
# define Utility_HPP
# include "Hacks.hpp"

class Utility {
public:
	static bool match(stringstream &ss, string target);

private:
	Utility();
	Utility(const Utility &);
	Utility &operator=(const Utility &);
	~Utility();
};

#endif
