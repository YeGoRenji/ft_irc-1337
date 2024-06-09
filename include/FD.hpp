#ifndef FD_HPP
# define FD_HPP
# include <iostream>
# include <unistd.h>
# include <Hacks.hpp>
# include <algorithm>

using std::string;

class FD {
public:
	FD();
	FD(int fd);
	FD(const FD &);
	FD &operator=(const FD &);
	~FD();
	FD &operator<<(std::string);
	void operator>>(std::string&);
	int getValue();
	void setValue(int fd);
	
private:
	int fd;
	string remainder;
	
};

#endif
