#include "Server.hpp"

int main (void) {

	Server srv(6969);

	srv.start();
	
	return 0;
}
