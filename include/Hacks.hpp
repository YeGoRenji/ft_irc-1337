#ifndef Hacks_HPP
# define Hacks_HPP

# include <iostream>
# include <sstream>
# include <cstring>
# include <cerrno>
# include <csignal>
# include <map>
# include <vector>
# include <algorithm>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/poll.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# define NAME(token) #token
# define DEBUG(printable) (cerr << NAME(printable) <<  ": " << printable << endl)

# define CONTAINS(buffer, str) (buffer.find((str)) != string::npos)
# define INCLUDES(cont, elt) (cont.find((elt)) != cont.end())

using std::cout;
using std::stringstream;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::vector;
using std::runtime_error;
using std::find;
using std::replace;
using std::min;
using std::pair;
using std::make_pair;
using std::exception;

#endif
