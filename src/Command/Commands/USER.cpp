#include "Commands.hpp"
#include "Utils.hpp"

void	USER(Server &server, User &user, Command &cmd)
{
	std::string										params;
	std::string										tmp;
	std::map<std::string, std::string>				configs;
	std::map<std::string, std::string>::iterator	ite;
	std::vector<std::string>						split;

	tmp = "";
	if (user.get_registered())
		user.send_msg(cmd, "462", "Unauthorized command (already registered)");
	else if (user.get_password() == server.get_password())
	{
		params = cmd.get_params();
		split = irc::split(params, " ");
		user.set_username(split[0]);
		user.set_hostname(split[2]);
		for (size_t i = 3; i < split.size(); i++)
		{
			if (tmp.size())
				tmp += " ";
			tmp += split[i];
		}
		user.set_realname(tmp);
	}
}
