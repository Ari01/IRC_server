#include "Commands.hpp"

/* GET_BROADCAST
** Gets the broadcast user list to send a NICK msg to
** Searches for user fd in all chans. If user is in a chat, add all users on this chan to the
** broadcast list. If a user is already on the broadcast list, do nothing.
** @param Server : server object
** @param User : the user to look for on channels
** @return : the list to broadcast the nick msg to */
std::map<int, User *>	get_broadcast(Server &server, User &user)
{
	std::map<std::string, Channel *>			chans;
	std::map<std::string, Channel *>::iterator	chan_ite;
	std::map<int, User *>						chan_users;
	std::map<int, User *>::iterator				users_ite;
	std::map<int, User *>						broadcast;

	broadcast[user.get_fd()] = &user;
	chans = server.get_channels();
	for (chan_ite = chans.begin(); chan_ite != chans.end(); chan_ite++)
	{
		chan_users = chan_ite->second->get_users();
		users_ite = chan_users.find(user.get_fd());
		if (users_ite != chan_users.end())
		{
			for (users_ite = chan_users.begin(); users_ite != chan_users.end(); users_ite++)
				broadcast.insert(std::make_pair(users_ite->first, users_ite->second));
		}
	}
	return (broadcast);
}

void	NICK(Server &server, User &user, Command &command)
{	
	if(!command.get_params().size())
		return user.send_msg(command, "431", ":No nickname given");
	std::vector<std::string>	params;
	std::map<int , User *>		s_users;
	std::map<int, User *>		broadcast;

	s_users = server.get_users();

	//NICK dchheang
	params = irc::split(command.get_params(), " ");
	
	
	std::string nickname = params[0];

	// check format
	if(!irc::isLetter(nickname[0]) && !irc::isSpecial(nickname[0]))
	{
		user.send_msg(command, "432", user.get_nickname() + " :" + nickname + " :Erroneous Nickname");
		return;
	}
	for(size_t i = 1; i < nickname.length(); i++)
	{
		if (!irc::isLetter(nickname[i]) && !irc::isSpecial(nickname[i]) && !irc::isDigit(nickname[i]))
		{
			user.send_msg(command, "432", user.get_nickname() + " :" + nickname + " :Erroneous Nickname");
			return;
		}
	}
	// check in use
	for (std::map<int, User *>::iterator ite = s_users.begin(); ite != s_users.end(); ite++)
	{
		if(nickname == ite->second->get_nickname())
		{
			user.send_msg(command, "433", " * " + nickname + " :Nickname is already in use");
			return ;
		}
	}

	//for loop
	if (!user.get_nickname().size())
		command.set_src("*");
	else
		command.set_src(user.get_nickname());
	//command.set_dsts(nickname);
	std::string buf = "NICK :" + nickname;

	broadcast = get_broadcast(server, user);
	for(std::map<int, User *>::iterator ite = broadcast.begin(); ite != broadcast.end(); ite++)
	{
		ite->second->send_msg(command, "", buf);
	}
	user.set_nickname(nickname);
	//broadcast to channel
	

	//send to client his new nickname || attention sur premiere conection
	//user.send_msg(command, "", "You're now known as \e[1m" + nickname + "\e[0m");
	return;
}
