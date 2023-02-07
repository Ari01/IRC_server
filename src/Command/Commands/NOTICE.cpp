#include "Commands.hpp"

void	NOTICE(Server &server, User &user, Command &cmd)
{
    if (!cmd.get_params().size())
		return;
    std::string recipient;
    std::map<std::string, Channel*> channels = server.get_channels();
    std::string msg;
    std::vector<std::string>    params = irc::split(cmd.get_params(), " ");
    std::map<int, User *>       s_users = server.get_users();
    std::vector<std::string>::iterator it = params.begin();

    recipient = *it++;

    if (it != params.end() && it->size())
        it->erase(0, 1);
    for (;it != params.end(); it++)
    {
        if (msg.size())
            msg += " ";
        msg += *it;
    }

    if(*recipient.begin() == '#')
    {
        std::map<std::string, Channel *>::iterator ite;
        ite = channels.find(recipient);
        if(ite != channels.end())
        {
            Channel &channel = *(ite->second);
            std::map<int, User *> c_users = channel.get_users();
            if(channel.get_mode().find('n') != std::string::npos)
                if(channel.get_users().find(user.get_fd()) == channel.get_users().end())
                    return;
            
            if(channel.get_mode().find('m') != std::string::npos)
                if(user.get_mode().find('o') == std::string::npos || user.get_mode().find('O') == std::string::npos)
                    return;
            
            for(std::map<int, User *>::iterator ite = c_users.begin(); ite != c_users.end(); ite++)
            {
                if (ite->second->get_nickname() != user.get_nickname())
                {
                    std::string buf = "NOTICE " + recipient + " :" + msg;
                    cmd.set_src(user.get_prefix());
                    cmd.set_dsts("");
                    ite->second->send_msg(cmd, "", buf);
                }
            }
        }
    }
    else
    {
        /* User to User case
        */
        for (std::map<int, User *>::iterator ite = s_users.begin(); ite != s_users.end(); ite++)
        {
            if(ite->second->get_nickname() == recipient)
            {
                std::string buf = "NOTICE " + recipient + " :" + msg;
                cmd.set_src(user.get_prefix());
                cmd.set_dsts("");
                ite->second->send_msg(cmd, "", buf);
                break;
            }
        }

    }
}