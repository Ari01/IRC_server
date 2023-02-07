#include "Commands.hpp"

void    KICK(Server &serv, User &user, Command &cmd)
{
    std::vector<std::string>	params = irc::split(cmd.get_params(), " ");
	if (params.size() < 2)
        return user.send_msg(cmd, "461", "KICK :Not enough parameters");
    
	std::vector<std::string>	channels;
	std::vector<std::string>	k_users = irc::split(params[1], ",");
	

	if (params[0].find(",") != std::string::npos)
		channels = irc::split(params[0], ",");
	else
		channels.push_back(params[0]);
	
	cmd.set_src(user.get_prefix());
	
	for(std::vector<std::string>::iterator c_ite = channels.begin(); c_ite != channels.end(); c_ite++)
	{
		if(!serv.get_channel(*c_ite))
		{
			cmd.set_dsts("");
			user.send_msg(cmd, "403",user.get_nickname() + " " + *c_ite + " :No such channel");
			continue;
		}
		Channel &channel = *(serv.get_channel(*c_ite));
		if(channel.get_user_mode(user).find('o') == std::string::npos)
		{
			user.send_msg(cmd, "482", *c_ite + " :You're not channel operator");
			continue;
		}
		for(std::vector<std::string>::iterator u_ite = k_users.begin(); u_ite != k_users.end(); u_ite++)
		{
			if(serv.get_user(*u_ite))
			{
				User &k_user = *(serv.get_user(*u_ite));
				if(channel.get_users().find(k_user.get_fd()) == channel.get_users().end())
				{
					cmd.set_src("");
					user.send_msg(cmd, "441", *u_ite + " " + *c_ite + " :They aren't on that channel");
					cmd.set_src(user.get_prefix());
					continue;
				}
				std::cout <<k_user.get_nickname() +  "kicked" << std::endl;
				cmd.set_dsts("");
				channel.broadcast(cmd, "KICK " + *c_ite + " " + *u_ite + " :" + params[1]);
				channel.remove_user(k_user);
			}
			else
			{
				user.send_msg(cmd, "441", *u_ite + " " + *c_ite + " :They aren't on that channel");
				continue;
			}	
		}
	}


}
