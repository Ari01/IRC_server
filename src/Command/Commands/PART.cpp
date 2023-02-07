#include "Commands.hpp"

void	PART(Server &server, User &user, Command &cmd)
{
    if (!cmd.get_params().size())
        return user.send_msg(cmd, "461", "PART :Not enough parameters");
    std::vector<std::string>    param_chan;
    std::string                 param;
    Channel                     *act_chan;


    param_chan = irc::split(cmd.get_params(), " ");
    for (std::vector<std::string>::iterator it = ++param_chan.begin(); it != param_chan.end(); it++)
    {
        if (param.size())
            param += " ";
        param += *it;
    }
    param_chan = irc::split(param_chan[0], ",");
    std::cout << "sda" << param << std::endl;
    for (std::vector<std::string>::iterator it = param_chan.begin(); it != param_chan.end(); it++)
    {
        if (server.get_channels().find(*it) == server.get_channels().end())
        {
            cmd.set_src(user.get_prefix());
            cmd.set_dsts("");
            user.send_msg(cmd,  "403 " + user.get_nickname() + " " + *it + " :No such channel", "");
            continue;
        }
        act_chan = server.get_channels().find(*it)->second;
        std::map<int, User *>   users = act_chan->get_users();
        if (users.find(user.get_fd()) == users.end())
        {
            cmd.set_src(user.get_prefix());
            cmd.set_dsts("");
            user.send_msg(cmd,  "442 " + user.get_nickname() + " " + *it + " :You're not on that channel", "");
            continue;
        }
        cmd.set_src(user.get_prefix());
        cmd.set_dsts("");
        act_chan->broadcast(cmd, "PART " + act_chan->get_name() + (param.size() ? " " + param : ""));
        act_chan->remove_user(user);
        if (!act_chan->get_users().size())
            server.delete_channel(*it);
    }
}