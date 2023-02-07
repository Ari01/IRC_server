NAME =		ircserv

SRCS =		src/Server/Server.cpp \
			src/Channel/Channel.cpp \
			src/User/User.cpp \
			src/Command/Command.cpp\
			utils/Utils.cpp\
			src/Command/Commands/JOIN.cpp \
			src/Command/Commands/NICK.cpp \
			src/Command/Commands/LUSERS.cpp \
			src/Command/Commands/MOTD.cpp \
			src/Command/Commands/USER.cpp \
			src/Command/Commands/OPER.cpp\
			src/Command/Commands/PING.cpp \
			src/Command/Commands/PASS.cpp \
			src/Command/Commands/QUIT.cpp \
			src/Command/Commands/MODE.cpp \
			src/Command/Commands/TOPIC.cpp \
			src/Command/Commands/INVITE.cpp \
			src/Command/Commands/PRIVMSG.cpp\
			src/Command/Commands/PART.cpp\
			src/Command/Commands/WHOIS.cpp\
			src/Command/Commands/WHO.cpp\
			src/Command/Commands/KICK.cpp\
			src/Command/Commands/NOTICE.cpp\
			src/main.cpp \
			utils/Utils.cpp

OBJS =		$(SRCS:.cpp=.o)

TESTS =		Server.cpp \
			src/Command/Command.cpp

OBJS =		$(SRCS:.cpp=.o)

CC =		c++
CFLAGS =	-Wall -Wextra -Werror -std=c++98 -g
INCLUDES =	-I . -I src/Command -I src/User -I src/Server -I src/Channel -I src/Command/Commands -I utils/

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

server:		$(OBJS)
			$(CC) $(CFLAGS) $(INCLUDES) $^ tests/Server_test.cpp -o $@

command:	$(OBJS)
			$(CC) $(CFLAGS) $(INCLUDES) $^ tests/Command_test.cpp -o $@

.cpp.o:
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $(<:.cpp=.o)

clean:
			rm -rf $(OBJS) $(TEST_OBJS)

fclean:		clean
			rm -rf $(NAME) server command */.*.swp */*/.*.swp

re:			fclean all

.PHONY:		all clean fclean re
