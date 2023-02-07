#include "Commands.hpp"

//a modifier type dehannel en founctioon du om @#& suprimer save channel '!'

void    JOIN(Server &serv, User &user, Command &cmd)
{
            if (!cmd.get_params().size())
                return user.send_msg(cmd, "461", "JOIN :Not enough parameters");
            std::vector<std::string>                param;
            std::vector<std::string>                chan_name;
            std::vector<std::string>                pass_list;
            std::vector<std::string>::iterator      it_pass;
            std::string                             prefix;
            std::string                             pass;
            Channel                                 *chan;

            std::cout << "//////////////////////////////////////////JOIN///////////////////" << std::endl;
            std::cout << cmd.get_params() << std::endl;
            param = irc::split(cmd.get_params(), " ");
            if (param.size() >= 1)
                chan_name = irc::split(param[0], ",");
            if (param.size() >= 2)
                pass_list = irc::split(param[1], ",");
            it_pass = pass_list.begin();
            std::cout << "name: " << param[0] << std::endl;
            for (std::vector<std::string>::const_iterator it = chan_name.begin(); it != chan_name.end(); it++)
            {
                std::cout << "name: " << *it << std::endl;
                if (!irc::is_channel_name(*it))
                {
                    cmd.set_src(user.get_prefix());
                        cmd.set_dsts("");
                        user.send_msg(cmd,  "476 " + user.get_nickname() + " " + *it + " :Bad Channel Mask", ""); //error chan name invalid
                        continue;
                }
                if (serv.get_channels().find(*it) == serv.get_channels().end())
                {
                    chan = serv.add_new_channel(*it, "", "", user);
                }
                else
                {
                    chan = (serv.get_channels().find(*it))->second;
                    if (it_pass != pass_list.end())
                        pass = *it_pass++;
                    else
                        pass = "";
                    if (chan->get_mode().find("k") != std::string::npos && pass != chan->get_pass())
                    {
                        cmd.set_src(user.get_prefix());
                        cmd.set_dsts("");
                        user.send_msg(cmd,  "475 " + user.get_nickname() + " " + chan->get_name() + " :Cannot join channel (+k)", ""); //error mauvaise key ERR_BADCHANNELKEY modif taille 512
                        continue;
                    }
                    if (chan->get_mode().find("l") != std::string::npos && chan->get_users().size() >= static_cast<size_t>(atoi(chan->get_user_max().c_str())))
                    {
                        cmd.set_src(user.get_prefix());
                        cmd.set_dsts("");
                        user.send_msg(cmd,  "471 " + user.get_nickname() + " " + chan->get_name() + " :Cannot join channel (+l)", ""); //error serv full ERR_CHANNELISFULL modif taille 512
                        continue;
                    }
					// reply invite ok
                    if (chan->get_mode().find("i") != std::string::npos && !chan->is_invited(user) && user.get_mode().find("o") == std::string::npos)
                    {
						cmd.set_src(user.get_prefix());
                        cmd.set_dsts("");
                        user.send_msg(cmd,  "473 " + user.get_nickname() + " " + chan->get_name() + " :Cannot join channel (+i)", "");
						continue ;
                    }
                    if (chan->is_ban(user))
                    {
                        cmd.set_src(user.get_prefix());
                        cmd.set_dsts("");
                        user.send_msg(cmd,  "474 " + user.get_nickname() + " " + chan->get_name() + " :Cannot join channel (+b)", "");
						continue ;
                    }
                    chan->remove_invited(user);
                    chan->add_user(user);
                }
				cmd.set_src(user.get_prefix());
                if (chan->get_topic().size())
					user.send_msg(cmd, "332", user.get_nickname() + " " + chan->get_name() + " :" + chan->get_topic());
				user.send_msg(cmd, "353", irc::user_list(user, *chan, chan->get_users()));
				user.send_msg(cmd, "366", user.get_nickname() + " " + chan->get_name() + " :End of /NAMES list");
                chan->broadcast(cmd, "JOIN :" + chan->get_name());
            }
}
