#include "Commands.hpp"

void	PASS(Server &server, User &user, Command &cmd)
{
	(void)server;

	if (user.get_registered())
		user.send_msg(cmd, "462", "Unauthorized command (already registered)");
	else
		user.set_password(cmd.get_params());
}
