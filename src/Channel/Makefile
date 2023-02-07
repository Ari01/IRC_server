NAME =		irc

SRCS =		Channel.cpp ../User/User.cpp\
			main.cpp

OBJS =		$(SRCS:.cpp=.o)

CC =		c++
CFLAGS =	-Wall -Wextra -Werror -std=c++98

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) -I ./ -I ../User/ $^ -o $@

.cpp.o:
			$(CC) $(CFLAGS) -I ./ -I ../User/ -c $< -o $(<:.cpp=.o)

clean:
			rm -rf $(OBJS)

fclean:		clean
			rm -rf $(NAME)

re:			fclean all

.PHONY:		all clean fclean re