#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <cstdlib>
# include <sstream>
# include <string>
# include "User.hpp"
# include "Server.hpp"
# include "Channel.hpp"
# include "Command.hpp"
# include "Utils.hpp"

# define CHAN_MODE "Oovlkbpsitnm"

// CONNECTION
void	NICK(Server &server, User &user, Command &cmd);
void	MODE(Server &server, User &user, Command &command);
void	USER(Server &server, User &user, Command &cmd);
void	LUSERS(Server &server, User &user, Command &cmd);
void	MOTD(Server &server, User &user, Command &cmd);
void	PASS(Server &server, User &user, Command &cmd);
void	OPER(Server &server, User &user, Command &cmd);
void	QUIT(Server &server, User &user, Command &command);

// MISCALLANEOUS
void	PING(Server &server, User &user, Command &cmd);

// CHANNEL
void    JOIN(Server &serv, User &user, Command &cmd);
void    PART(Server &serv, User &user, Command &cmd);
void	TOPIC(Server &serv, User &user, Command &cmd);
void	INVITE(Server &server, User &user, Command &cmd);
void	KICK(Server &serv, User &user, Command &cmd);

//SENDING
void	PRIVMSG(Server &server, User &user, Command &command);
void	NOTICE(Server &server, User &user, Command &cmd);

//USER
void	WHOIS(Server &serv, User &user, Command &cmd);
void	WHO(Server &serv, User &user, Command &cmd);

#endif
