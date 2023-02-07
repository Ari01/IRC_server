#include "Commands.hpp"

/* INVITE
** Invites another user on a channel
** The channel doesn't have to exit. But if it does, only users on that channel can send an invite
** @replies 
	401 ERR_NOSUCHNICK if user doesn't exist
	443 ERR_USERONCHANNEL if user already on channel
	442 ERR_NOTONCHANNEL if user inviting is not on the channel
	482 ERR_CHANOPRIVSNEEDED if chan mode is +i and user inviting doesnt have op privileges
	341 RPL_INVITING to user inviting if invite succeeded
	/REPLY to user invited if invite succeeded */
void	INVITE(Server &server, User &user, Command &cmd)
{
	std::vector<std::string>					params;
	std::map<std::string, Channel *>			chans;
	std::map<std::string, Channel *>::iterator	chan_ite;
	std::map<int, User *>						chan_users;
	Channel										*chan;
	User										*toInvite;
	
	params = irc::split(cmd.get_params(), " ");
	chans = server.get_channels();
	if (params.size() == 2)
	{
		toInvite = server.get_user(params[0]);
		if (toInvite == NULL)
		{
			cmd.set_dsts(user.get_nickname());
			user.send_msg(cmd, "401", params[0]);
			return ;
		}
		else
		{
			chan_ite = chans.find(params[1]);
			if (chan_ite != chans.end())
			{
				chan = chan_ite->second;
				chan_users = chan->get_users();
				cmd.set_dsts(user.get_nickname());
				if (chan_users.find(user.get_fd()) == chan_users.end())
				{
					user.send_msg(cmd, "442", chan->get_name() + " :You're not on that channel");
					return ;
				}
				else if (chan_users.find(toInvite->get_fd()) != chan_users.end())
				{
					user.send_msg(cmd, "443", toInvite->get_nickname() + " " + chan->get_name() + " is already on channel");
					return ;
				}
				else if (chan->get_user_mode(user).find('o') == std::string::npos
					&& chan->get_user_mode(user).find('O') == std::string::npos)
				{
					cmd.set_dsts("");
					user.send_msg(cmd, "482", user.get_nickname() + " " + chan->get_name() + " You're not channel operator");
					return ;
				}
				chan->add_invited(*toInvite);
			}
			cmd.set_src(user.get_nickname());
			cmd.set_dsts(params[0]);
			user.send_msg(cmd, "341 " + params[0], params[1]);
			toInvite->send_msg(cmd, "INVITE ", params[1]);
		}
	}
	else
	{
		return user.send_msg(cmd, "461", "INVITE :Not enough parameters");
	}
}
