CXX = c++

NAME = TypeIdentifier

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = Client.cpp \
	   FD.cpp \
       Server.cpp \
       Channel.cpp \
       main.cpp \

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: clean all

.PHONY: clean fclean re bonus all
