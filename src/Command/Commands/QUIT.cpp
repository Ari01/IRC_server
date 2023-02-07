#include "Command.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "Commands.hpp"

 #include <sys/types.h>
 #include <sys/socket.h>

/* SEGFAULT
** connect user1, user2, join same channel, ctrl+n to get out of channel window, /exit */
void QUIT(Server &server, User &user, Command &command)
{
    std::map<std::string, Channel *> s_channel = server.get_channels();
    std::string msg = command.get_params();
    bool inChannel = false;

    for(std::map<std::string, Channel *>::iterator c_ite = s_channel.begin(); c_ite != s_channel.end(); c_ite++)
    {
        std::map<int, User *> c_users = c_ite->second->get_users();
        for(std::map<int, User *>::iterator u_ite = c_users.begin(); u_ite != c_users.end(); u_ite++)
        {
            if (u_ite->second->get_nickname() == user.get_nickname())
                inChannel = true;
        }
        if(inChannel == true)
        {
            for(std::map<int, User *>::iterator u_ite = c_users.begin(); u_ite != c_users.end(); u_ite++)
            {
                if(u_ite->second->get_nickname() != user.get_nickname())
                {
                    std::string buf = "QUIT " + msg;
                    command.set_src(user.get_prefix());
                    command.set_dsts("");
                    u_ite->second->send_msg(command, "", buf);
                }
            }
        }
    }      
    user.send_msg(command, "", "ERROR");
}
