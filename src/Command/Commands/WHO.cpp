#include "Commands.hpp"
/* 
	La fonction ne marche pas, il faudra rajouter dans les user un membre "last channel" qui est remplace par #debug dans cette fonction
*/
void	WHO(Server &serv, User &user, Command &cmd)
{
	if (!cmd.get_params().size())
        return user.send_msg(cmd, "461", "WHO :Not enough parameters");
    std::vector<std::string>	params = irc::split(cmd.get_params(), " ");
	std::map<int, User *>		users = serv.get_users();
    bool						star = false;
    bool						op = false;
    size_t						pos = 0;

	cmd.set_src(user.get_prefix());
    if((pos = params[0].find("*")) != std::string::npos)
		star = true;
	if(params.size() > 1 && params[1] == "o")
		op = true;
	if(params[0][0] == '#')
	{
		if(serv.get_channels().find(params[0]) == serv.get_channels().end())
		{
			return user.send_msg(cmd, "315", user.get_nickname() + " " + user.get_username() + " :End of /WHO list");
		}
		if (serv.get_channels().find(params[0]) != serv.get_channels().end())
		{
			Channel &channel = *(serv.get_channels().find(params[0])->second);

			for (std::map<int, User *>::iterator ite = users.begin(); ite != users.end(); ite++)
			{
				if(channel.get_users().find(ite->second->get_fd()) != channel.get_users().end())
				{
					if(user.get_mode().find('o') == std::string::npos && ite->second->get_mode().find('i') != std::string::npos)
						continue;

					std::string state = "H";

					if(ite->second->get_mode().find('o') != std::string::npos)
						state += "@";
					if(op && ite->second->get_mode().find('o') != std::string::npos)
						user.send_msg(cmd, "352", user.get_nickname() + " " +  "#debug " + ite->second->get_username() + " " + ite->second->get_hostname() + " " + "" + " " + ite->second->get_nickname() + " " + state + " :0 " + ite->second->get_realname());
					else if(!op)
						user.send_msg(cmd, "352",user.get_nickname() + " " +  "#debug " + ite->second->get_username() + " " + ite->second->get_hostname() + " " + "" + " " + ite->second->get_nickname() + " " + state + " :0 " + ite->second->get_realname());
				}
			}
		}
		return user.send_msg(cmd, "315",user.get_nickname() + " " + user.get_username() + " :End of /WHO list");
	}
	for (std::map<int, User *>::iterator ite = users.begin(); ite != users.end(); ite++)
	{
		std::string state = "H";

		if(ite->second->get_mode().find('o') != std::string::npos)
			state += "@";
		if (op && ite->second->get_mode().find("o") != std::string::npos && (ite->second->get_hostname() == params[0] || ite->second->get_realname() == params[0] || ite->second->get_nickname() == params[0]))
			user.send_msg(cmd, "352",user.get_nickname() + " " +  "#debug " + ite->second->get_username() + " " + ite->second->get_hostname() + " " + "" + " " + ite->second->get_nickname() + " " + state + " :0 " + ite->second->get_realname());
		else if (!op && (ite->second->get_hostname() == params[0] || ite->second->get_realname() == params[0] || ite->second->get_nickname() == params[0]))
			user.send_msg(cmd, "352",user.get_nickname() + " " +  "#debug " + ite->second->get_username() + " " + ite->second->get_hostname() + " " + "" + " " + ite->second->get_nickname() + " " + state + " :0 " + ite->second->get_realname());
		else if (op && ite->second->get_mode().find("o") != std::string::npos && star && (params[0].substr(0, pos) == ite->second->get_hostname().substr(0, pos) || params[0].substr(0, pos) == ite->second->get_realname().substr(0, pos) || params[0].substr(0, pos) == ite->second->get_nickname().substr(0, pos)))
			user.send_msg(cmd, "352",user.get_nickname() + " " +  "#debug " + ite->second->get_username() + " " + ite->second->get_hostname() + " " + "" + " " + ite->second->get_nickname() + " " + state + " :0 " + ite->second->get_realname());
		else if (!op && star && (params[0].substr(0, pos) == ite->second->get_hostname().substr(0, pos) || params[0].substr(0, pos) == ite->second->get_realname().substr(0, pos) || params[0].substr(0, pos) == ite->second->get_nickname().substr(0, pos)))
			user.send_msg(cmd, "352",user.get_nickname() + " " +  "#debug " + ite->second->get_username() + " " + ite->second->get_hostname() + " " + "" + " " + ite->second->get_nickname() + " " + state + " :0 " + ite->second->get_realname());
	}
}