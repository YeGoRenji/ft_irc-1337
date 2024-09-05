CXX := c++

NAME := ircbot

CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -g #-fsanitize=address #-ggdb3

OBJSFOLDER := objs/

INCLUDE := -Iinclude

SRCS := Game.cpp   \
    	FD.cpp      \
    	Utility.cpp  \
		Bot.cpp 	  \
		XOBot.cpp

OBJS := $(SRCS:.cpp=.o)

GLOBAL_HEADERS := include/Hacks.hpp

OBJS := $(addprefix $(OBJSFOLDER), $(OBJS))

all: objs $(NAME)

objs:
	@mkdir objs

$(NAME): $(OBJS) objs/main.o
	$(CXX) $^ $(CXXFLAGS) -o $@ $(INCLUDE)
	@echo "👍🏿"

$(OBJSFOLDER)%.o: src/%.cpp include/%.hpp
	@echo "⚙️  Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OBJSFOLDER)main.o: main.cpp
	@echo "⚙️  Compiling MAIN"
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -c main.cpp -o $@

clean:
	rm -rf $(OBJS) objs/main.o

fclean: clean
	rm -rf $(NAME)
	find . -name "*~" -delete
	find . -name "*.swp" -delete

re: clean all

.PHONY: clean fclean re all
