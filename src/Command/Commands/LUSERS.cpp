#include "Commands.hpp"
#include "Utils.hpp"

void	LUSERS(Server &server, User &user, Command &cmd)
{
	int								nusers;
	int								nops;
	int								nunknown;
	int								nchannels;
	std::map<int, User *>::iterator	ite;
	std::map<int, User *>			users;
	User							*current_user;

	users = server.get_users();
	nusers = users.size();
	nunknown = 0;
	nops = 0;
	nchannels = server.get_channels().size();
	for (ite = users.begin(); ite != users.end(); ite++)
	{
		current_user = ite->second;
		if (!current_user->get_registered())
			nunknown++;
		if (current_user->get_mode().find("o") != std::string::npos)
			nops++;
	}
	user.send_msg(cmd, "251", "There are " + irc::atoi(nusers) + " users and 0 services on 1 servers");
	if (nops)
		user.send_msg(cmd, "252", irc::atoi(nops) + " :operator(s) online");
	if (nunknown)
		user.send_msg(cmd, "253", irc::atoi(nunknown) + " :unknown connection(s)");
	if (nchannels)
		user.send_msg(cmd, "254", irc::atoi(nchannels) + " :channels formed");
	user.send_msg(cmd, "255", "I have " + irc::atoi(nusers) + " clients and 1 servers");
}
