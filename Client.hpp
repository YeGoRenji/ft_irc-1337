#ifndef Client_HPP
# define Client_HPP
# include "FD.hpp"
# include "Hacks.hpp"
# include <unistd.h>

class Client {
public:
	Client(int fd);
	~Client();
	int getFd();
	FD &getFdObject();
	void disconnect();
	
private:
	Client();
	FD fdObject;
	bool isAuthed;

};

#endif
