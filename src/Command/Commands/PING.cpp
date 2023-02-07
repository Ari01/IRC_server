#include "Commands.hpp"

void	PING(Server &server, User &user, Command &cmd)
{
	(void)server;
	user.send_msg(cmd, "", "PONG");
}
