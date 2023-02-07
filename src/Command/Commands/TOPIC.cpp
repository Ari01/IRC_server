#include "Commands.hpp"
#include "Utils.hpp"

/* TOPIC
** si 2 parametres, on change le topic du channel
** sinon, on demande le topic actuel du channel
** @REPLIES
** 461 : not enough params
** 332 : RPL_TOPIC
** 331 : RPL_NOTOPIC
** 482 : ERR_PRIVOPSNEEDED */

void	TOPIC(Server &server, User &user, Command &cmd)
{
	std::map<std::string, Channel *>			chan_map;
	std::map<int, User *>						chan_users;
	std::map<int, User *>::iterator				user_ite;
	std::string									user_mode;
	std::string									chan_name;
	std::vector<std::string>					split;
	Channel										*chan;						

	if (!cmd.get_params().size())
        return user.send_msg(cmd, "461", "TOPIC :Not enough parameters");
	split = irc::split(cmd.get_params(), " ");
	chan_name = split[0];
	chan_map = server.get_channels();
	cmd.set_src(user.get_prefix());
	cmd.set_dsts("");

	if (chan_name.size())
	{
		chan = server.get_channel(chan_name);
		if (chan)
		{
			std::cout << "channel found\n";
			chan_users = chan->get_users();
			user_ite = chan_users.find(user.get_fd());
			if (user_ite != chan_users.end())
			{
				std::cout << "user found \n";
				user_mode = chan->get_user_mode(user);
				if (split.size() == 1)
				{
					std::cout << "no params\n";
					if (chan->get_topic().size())
						user.send_msg(cmd, "332", chan_name + " :" + chan->get_topic());
					else
						user.send_msg(cmd, "331", chan_name + " :No topic is set");
				}
				else if (chan->get_mode().find('t') != std::string::npos)
				{
					std::cout << "chan mode t found\n";
					if (user_mode.find('o') == std::string::npos
						&& user_mode.find('O') == std::string::npos)
					{
						cmd.set_src("");
						user.send_msg(cmd, "482", user.get_nickname() + " " + chan_name + " You're not channel operator");
					}
					else
					{
						chan->set_topic(split[1].substr(1));
						std::cout << "topic is " << chan->get_topic() << std::endl;
						chan->broadcast(cmd, "TOPIC " + chan_name + " :" + chan->get_topic());
					}
				}
				else
				{
					std::cout << "chan mode t not found\n";
					chan->set_topic(split[1].substr(1));
					std::cout << "topic is " << chan->get_topic() << std::endl;
					chan->broadcast(cmd, "TOPIC " + chan_name + " :" + chan->get_topic());
				}
			}
			else
				user.send_msg(cmd, "442", chan_name + " :You're not on that channel");
		}
		else
			user.send_msg(cmd, "442", chan_name + " :You're not on that channel");
	}
	else
		std::cout << "no chan name found\n";
}
