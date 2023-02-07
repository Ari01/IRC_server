#include "Commands.hpp"

void	WHOIS(Server &serv, User &user, Command &cmd)
{
    if (!cmd.get_params().size())
        return user.send_msg(cmd, "461", "WHOIS :Not enough parameters");
    std::string                         channels_names;
    std::map<std::string, Channel *>    channels = serv.get_channels();
    std::map<int, User *>               users = serv.get_users();
    std::vector<std::string>            params = irc::split(cmd.get_params(), " ");

    for(std::map<int, User *>::iterator u_ite = users.begin(); u_ite != users.end(); u_ite++)
    {
        if(params[0] == u_ite->second->get_nickname())
        {
            cmd.set_src(user.get_prefix());
            user.send_msg(cmd, "311", u_ite->second->get_nickname() + " " + u_ite->second->get_nickname()  + " " + u_ite->second->get_username() + " " + u_ite->second->get_hostname() + " * :" + u_ite->second->get_realname());
            for (std::map<std::string, Channel *>::iterator c_ite = channels.begin(); c_ite != channels.end(); c_ite++)
            {
                if(user.get_mode().find('o') != std::string::npos || (c_ite->second->get_users().find(u_ite->second->get_fd()) != c_ite->second->get_users().end() && u_ite->second->get_mode().find("i") == std::string::npos) )
                    channels_names += c_ite->second->get_name() + " ";
            }
            if (channels_names.length())
                user.send_msg(cmd, "319", params[0] + " :" + channels_names);
            if(u_ite->second->get_mode().find('o') != std::string::npos)
                user.send_msg(cmd, "313", params[0] + " :is an IRC operator");
            return user.send_msg(cmd, "318", params[0] + " :End of /WHOIS list");
        }
    }
    user.send_msg(cmd, "401", params[0] + " :No such nick/channel");
    return user.send_msg(cmd, "318", params[0] + " :End of /WHOIS list");
}