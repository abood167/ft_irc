NAME = ircserv
SRC =  Channel.cpp Command.cpp Server.cpp User.cpp Utils.cpp  main.cpp
OBJS = ${SRC:.cpp=.o}

CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS)  -o $(NAME)

clean:
	@$(RM)  $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re%