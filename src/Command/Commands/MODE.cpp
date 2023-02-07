#include "Command.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "Commands.hpp"
#include "Utils.hpp"

/**********************************************/
/***************** CHAN MODE ******************/
/**********************************************/
	std::string    mode_chan_reply(std::string mode, std::vector<std::string> params)
	{
		bool minus = false;
		std::string min;
		std::string plus;
		std::string msg;

		for (std::string::iterator it = mode.begin(); it != mode.end(); it++)
		{
			if (*it == '-')
				minus = true;
			else if (*it == '+')
				minus = false;
			else if (minus && *it != ' ' && *it != '+' && *it != '-')
				min += *it;
			else if (!minus && *it != ' ' && *it != '+' && *it != '-')
				plus += *it;
		}
		std::cout << "seg" << std::endl;
		if (plus.empty() && min.empty())
			return msg;
		if (!plus.empty() && plus.length())
			msg += "+" + plus;
		if (!min.empty() && min.length())
			msg += "-" + min;
		for (std::vector<std::string>::iterator ite = params.begin(); ite != params.end(); ite++)
		{
			if (!ite->empty() && ite->length())
			{
				msg += " " + *ite;
			}
		}
		return msg;
	}

	//set/unset mode 'mode' type a return false si le mode n' est pas modifier, true: le mode est modifier
	bool    ft_mode_a(Channel *chan, bool minus, std::string arg, char mode, std::string *reply)
	{
		int                     fd;
		std::map<int, User *>   users = chan->get_users();

		if (arg.empty() || !arg.size())
			return false;
		fd = irc::find_user(chan->get_users(), arg);
		std::cout << "fd" << fd << std::endl;
		if (fd)
		{
			std::cout << "user chan mode:" << chan->get_user_mode(*users[fd]) << std::endl;
			if (!minus)
			{
				if (!chan->set_user_mode(*users[fd], mode))
					return false;
				*reply += '+';
				*reply += mode;
			}
			else if (minus)
			{
				if (!chan->unset_user_mode(*users[fd], mode))
					return false;
				*reply += '-';
				*reply += mode;
			}
			return true;
		}
		return false;
	}

	//set/unset 'mode' type b . return false si aucun mode set/unset true si un mode est set/unset
	bool    ft_mode_b(Channel *chan, bool minus, std::string arg, char mode, std::vector<std::string> &reply_arg)
	{
		std::string str;
		int nb;
		std::stringstream stream;

		if (!minus && (arg.empty() || !arg.size()))
			return false;
		std::cout << "mode b" << std::endl;
		if (!minus)
		{
			nb = atoi(arg.c_str());
			if (nb == 0)
				nb = 1;
			if (chan->set_mode(str + mode) || (mode == 'k' && (chan->get_pass() != arg)) || (mode == 'l' && ((atoi(chan->get_user_max().c_str()) != nb))))
			{
				if (mode == 'k')
				{
					chan->set_pass(arg);
					reply_arg.push_back(arg);
				}
				else if (mode == 'l')
				{
					stream << nb;
					stream >> str;
					chan->set_user_max(str);
					reply_arg.push_back(str);
				}
				return true;
			}
			return false;
		}
		else if (minus)
		{
			if (!chan->unset_mode(str + mode))    
				return false;
		}
		return true;
	}

	bool    ft_chan_mode_toggle(Channel *chan, bool minus, char mode, std::string *reply)
	{
		std::string str;

		if (!minus)
		{
			 if (mode == 'p')
			{
				if(chan->unset_mode("s"))
					*reply += "-s";
			}
			if (mode == 's')
			{
				if(chan->unset_mode("p"))
					*reply += "-p";
			}
			if (!chan->set_mode(str + mode))
				return false;
			*reply += '+';
			*reply += mode;
		}
		else if (minus)
		{
			if (!chan->unset_mode(str + mode))
				return false;
			*reply += '-';
			*reply += mode;
		}
		return true;
	}

	bool    chan_mode_ban(Channel *chan, bool minus, std::string arg, std::vector<std::string> &reply_arg)
	{
		std::string banmask;

		if (arg.empty() || !arg.size())
			return false;
		banmask = irc::format_banmask(arg);
		if (!minus)
		{
			if (!chan->add_banmask(banmask))
				return false;
		}
		else if (minus)
		{
			if (!chan->remove_banmask(banmask))
				return false;
		}
		reply_arg.push_back(banmask);
		return true;
	}

	void mode_channel(Server &server, User &user, Command &cmd, std::vector<std::string> params)
	{
		std::string                         acceptChanMode = CHAN_MODE;
		std::string                         mode = "";
		std::vector<std::string>            reply_arg;
		std::vector<std::string>::iterator  arg;
		std::string                         act_arg;
		Channel                             *chan;
		bool                                minus = false;
		
		std::cout << "mode channel! " << std::endl;
		if (server.get_channels().find(params[0]) == server.get_channels().end())
		{
			cmd.set_src(user.get_prefix());
			cmd.set_dsts(params[0]);
			user.send_msg(cmd, "403", params[0]); //not a channel
			return;
		}
		chan = server.get_channels().find(params[0])->second;
		std::cout << "chan mode: " << chan->get_mode() << std::endl;
		if (params.size() == 1)
		{
			/*std::cout << "not enought params!" << params.size() << std::endl;
			user.send_msg(cmd, "221", chan->get_mode()); not enougth param*/
			cmd.set_src(user.get_prefix());
			cmd.set_dsts("");
			act_arg = irc::arg_list(*chan);
			user.send_msg(cmd,  "324 " + user.get_nickname() + " " + chan->get_name() + " " + chan->get_mode() + " " + act_arg, "");
			return;
		}
		if (chan->get_user_mode(user).find("o") == std::string::npos)
		{
			std::cout << "not oper!" << params.size() << std::endl;
			cmd.set_src(user.get_prefix());
			cmd.set_dsts("");
			user.send_msg(cmd, "482 " + user.get_nickname() + " " + params[0], ":You're not channel operator");
			return; //user don't have perm
		}
		if (params[1].find("b") != std::string::npos && params.size() == 2)
		{
			reply_arg = chan->get_banmask();
			cmd.set_src(user.get_prefix());
			cmd.set_dsts("");
			for (std::vector<std::string>::iterator it = reply_arg.begin(); it != reply_arg.end(); it++)
				user.send_msg(cmd, "367", user.get_nickname() + " " + chan->get_name() + " " + *it);
			cmd.set_src("");
			user.send_msg(cmd, "368", user.get_nickname() + " " + chan->get_name() + " :End of channel ban list");
			return;
		}
		//params.erase(params.begin(), params.begin() + 1);
		arg = params.begin() + 2;
		for (std::string::iterator it = params[1].begin(); it != params[1].end(); it++)
		{
			std::cout << "mode: " << *it << std::endl;
			if (*it == '-')
				minus = true;
			else if (*it == '+')
				minus = false;
			else if (acceptChanMode.find(*it) != std::string::npos)
			{
				if (*it == 'o' || *it == 'v')
				{
					if (arg != params.end())
						act_arg = *arg;
					else
						act_arg = "";
					if (ft_mode_a(chan, minus, act_arg, *it, &mode))
							reply_arg.push_back(*arg);
					if (arg != params.end())
						arg++;
				}
				else if (*it == 'k' || *it == 'l')
				{
					std::cout << "mode key/nb!" << std::endl;
					if (arg != params.end())
						act_arg = *arg;
					else
						act_arg = "";
					if (ft_mode_b(chan, minus, act_arg, *it, reply_arg))
					{
						if (!minus)

							mode += "+";
						else
							mode += "-";
						mode += *it;
					}
					if (!minus && arg != params.end())
							arg++;
				}
				else if (*it == 'b')
				{
					std::cout << "mode ban" << std::endl;
					if (arg != params.end())
						act_arg = *arg;
					else
						act_arg = "";
					if (chan_mode_ban(chan, minus, act_arg, reply_arg))
					{
						if (!minus)
							mode += "+";
						else
							mode += "-";
						mode += *it;
					}
					if (!minus && arg != params.end())
							arg++;
				}
				else
				{
					std::cout << "mode:" << *it << " mode chan: " << chan->get_mode() << std::endl;
					ft_chan_mode_toggle(chan, minus, *it, &mode);
				}
			}
		}
		cmd.set_src(user.get_prefix());
		cmd.set_dsts("");

		std::cout << "reply: " << mode << std::endl;
		mode = mode_chan_reply(mode, reply_arg);
		std::cout << "reply: " << mode << std::endl;
		if (mode.empty())
			return;
		chan->broadcast(cmd, "MODE " + chan->get_name() + " " + mode);
		std::cout << "chan mode " << chan->get_mode() << std::endl;
		return;
	}

/**********************************************/
/***************** USER MODE ******************/
/**********************************************/

	void	toggleMode(User &user, char sign, char mode)
	{
		std::string		user_mode;
		std::string		ret;
		size_t			pos;

		user_mode = user.get_mode();
		pos = user_mode.find(mode);
		if (sign == '-' && pos != std::string::npos)
		{
			user_mode.erase(pos, 1);
			user.set_mode(user_mode);
		}
		else if (sign == '+' && pos == std::string::npos)
		{
			if (mode != 'o' && mode != 'O')
				user.set_mode(user_mode + mode);
		}
	}

	// MODE gives mode of user
	// MODE +wi7
	void mode_user(Server &server, User &user, Command &cmd, std::vector<std::string> params)
	{
		std::string				mode_charset("+-oOi");
		std::string				mode;
		std::string::iterator	ite;
		char					sign;
		User					*dst;

		cmd.set_src(user.get_nickname());
		cmd.set_dsts(user.get_nickname());
		if (params.size())
		{
			dst = server.get_user(params[0]);
			if (!dst || user.get_fd() != dst->get_fd())
			{
				user.send_msg(cmd, "502", "Cannot change mode for other users");
				return ;
			}
			else if (params.size() > 1)
			{
				mode = params[1];
				for (ite = mode.begin() + 1; ite != mode.end(); ite++)
				{
					if (mode_charset.find(*ite) == std::string::npos)
					{
						user.send_msg(cmd, "501", "Unknown MODE flag");
						return ;
					}
				}
				for (ite = mode.begin(); ite != mode.end(); ite++)
				{
					if (*ite == '-' || *ite == '+')
						sign = *ite;
					else
						toggleMode(user, sign, *ite);
				}
			}
			user.send_msg(cmd, "221", user.get_mode());
		}
	}

/**********************************************/
/******************** /MODE *******************/
/**********************************************/
	void MODE(Server &server, User &user, Command &command)
	{
		if (!command.get_params().size())
                return user.send_msg(command, "461", "MODE :Not enough parameters");
		std::vector<std::string> params;
		std::string accepted_mode = "iws";

		params = irc::split(command.get_params(), " ");  
		/*  A user MODE command MUST only be accepted if both the sender of the
			message and the nickname given as a parameter are both the same
		*/
		std::cout << "param: " << command.get_params() << std::endl;
		std::cout << "src: " << command.get_src() << std::endl;
		std::cout << "dsts: " << command.get_dsts() << std::endl;
		//command.set_src(user.get_prefix());
		if (irc::is_channel_name(params[0]))
			mode_channel(server, user, command, params);
		else
			mode_user(server, user, command, params);
	}
