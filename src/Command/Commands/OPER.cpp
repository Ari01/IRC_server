#include "Commands.hpp"

void	OPER(Server &server, User &user, Command &cmd)
{
	
	if (!cmd.get_params().size())
        return user.send_msg(cmd, "461", "OPER :Not enough parameters");
	std::string					params;
	std::string					nick;
	std::string					pass;
	std::vector<std::string>	split;

	params = cmd.get_params();
	split = irc::split(params, " ");
	if(split.size() < 2)
		return user.send_msg(cmd, "461", "OPER :Not enough parameters");

	if (!split.size())
	{
		nick = user.get_nickname();
		pass = split[0];
	}
	else
	{
		nick = split[0];
		pass = split[1];
	}
	if (nick != server.get_config("oper_user") || pass != server.get_config("oper_password"))
		user.send_msg(cmd, "464", " :Password incorrect");
	else
	{
		if (user.add_mode("o"))
		{
			std::cout << "user mode is = " << user.get_mode();
			cmd.set_src(user.get_nickname());
			cmd.set_dsts(user.get_nickname());
			user.send_msg(cmd, "221", user.get_mode());
		}
		else
			std::cout << "user add mode false\n";
		cmd.set_src("");
		cmd.set_dsts("");
		user.send_msg(cmd, "381", user.get_nickname() + " :You are now an IRC operator");
	}
}
