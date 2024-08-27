#ifndef FD_HPP
# define FD_HPP
# include <unistd.h>
# include <Hacks.hpp>

using std::string;

class FD {
public:
	FD();
	FD(int fd);
	FD(const FD &);
	FD &operator=(const FD &);
	~FD();
	FD &operator<<(std::string);
	void operator>>(std::string& buffer);
	int getValue();
	void setValue(int fd);
	
private:
	int fd;
	string remainder;
	
};

#endif
