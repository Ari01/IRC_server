#include "Command.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "Commands.hpp"
#include "Utils.hpp"

#include <sys/types.h>
#include <sys/socket.h>


void PRIVMSG(Server &server, User &user, Command &command)
{
    std::string msg;
    std::string recipient;

    std::map<int, User *> s_users = server.get_users();
    std::map<std::string, Channel*> channels = server.get_channels();

    std::vector<std::string>    params = irc::split(command.get_params(), " ");
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

    command.set_dsts(recipient);
    command.set_src(user.get_prefix());
    if(!msg.size())
        return user.send_msg(command, "411", ":No recipient given ( PRIVMSG )");
    if(!recipient.size())
        return user.send_msg(command, "411", ":No recipient given ( PRIVMSG )");
    
    if(*recipient.begin() == '#')
    {
        /* User to channel
        */
        std::map<std::string, Channel *>::iterator ite; //

        ite = channels.find(recipient);
        if(ite != channels.end())
        {           
            Channel &channel = *(ite->second);
            std::map<int, User *> c_users = channel.get_users();
            
            // docs: n = Les utilisateurs hors du salon ne peuvent envoyer de PRIVMSGs au salon 
            if (channel.get_mode().find('n') != std::string::npos)
            {
                if(c_users.find(user.get_fd()) == c_users.end())
                {
                    user.send_msg(command, "404", channel.get_name() + " :Cannot send to channel");
                    return;
                }
            }

            //docs: Salon modéré (seuls les utilisateurs +vhoaq peuvent parler)
            if (channel.get_mode().find("m") != std::string::npos)
            {
                
                if (channel.get_user_mode(user).find("o") == std::string::npos && channel.get_user_mode(user).find("v") == std::string::npos)
                {
                    user.send_msg(command, "404", recipient + " :Cannot send to channel");
                    return;
                }
            }
            for(std::map<int, User *>::iterator ite = c_users.begin(); ite != c_users.end(); ite++)
            {
                if (ite->second->get_nickname() != user.get_nickname())
                {
                    std::string buf = "PRIVMSG " + recipient + " :" + msg;
                    command.set_src(user.get_prefix());
                    command.set_dsts("");
                    ite->second->send_msg(command, "", buf);
                }    
            }            
        }
        else
        {
            user.send_msg(command, "404", recipient + " :Cannot send to channel");
            return;
        }
       
    }
    else
    {
        /* User to User case
        */
        bool found = false;
        for (std::map<int, User *>::iterator ite = s_users.begin(); ite != s_users.end(); ite++)
        {
            if(ite->second->get_nickname() == recipient)
            {
                std::string buf = "PRIVMSG " + recipient + " :" + msg;
                command.set_src(user.get_prefix());
                command.set_dsts("");
                ite->second->send_msg(command, "", buf);
                found = true;
                break;
            }
        }
        if(!found)
		    user.send_msg(command, "401", recipient + " ");
    }
}
