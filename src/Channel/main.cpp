#include "User.hpp"
#include "Channel.hpp"
#include <iostream>

int main()
{
    std::string username("dwiranor");
    std::string nick("dwi");
    std::string uMode("super mode");
    User    me(0, nick, "hostname", username, uMode);
    std::string chanName("monchan");
    std::string topic("fan de topiqueur et autres");
    std::string chanMode("no modes");
    Channel  chan(chanName, topic, chanMode, me);

    std::cout << "name: " << chan.get_name() << std::endl;
    std::cout << "topic: " << chan.get_topic() << std::endl;
    std::cout << "mode: " << chan.get_mode() << std::endl;

    chan.set_mode("transforMode");
    chan.set_topic("topichange");
    chan.set_name("meilleur nom");
    std::cout << "name: " << chan.get_name() << std::endl;
    std::cout << "topic: " << chan.get_topic() << std::endl;
    std::cout << "mode: " << chan.get_mode() << std::endl;

    std::map<std::string, User> users(chan.get_users());

    for(std::map<std::string, User>::iterator it = users.begin(); it != users.end(); it++)
    {
        std::cout << "user: " << it->first << std::endl;
    }
    User    _new(0, "le nouveau", "hostname", "le S", "pas mode");
    std::cout << " ajout nouveau user" << std::endl;
    chan.add_user(_new);
    std::cout << " ajout nouveau user" << std::endl;
    User    autre_new(0, "le nouveau en mieux", "hostname", "Rush B", "rusian mode");
    chan.add_user(autre_new);
    std::cout << " ajout nouveau user" << std::endl;
    chan.add_user(autre_new);
    users = chan.get_users();
    std::cout << " ajout nouveau user" << std::endl;
    for(std::map<std::string, User>::iterator it = users.begin(); it != users.end(); it++)
    {
        std::cout << "user: " << it->first << " mode: " << it->second.get_mode() << std::endl;
    }
    chan.remove_user("le nouveau");
    users = chan.get_users();
    for(std::map<std::string, User>::iterator it = users.begin(); it != users.end(); it++)
    {
        std::cout << "user: " << it->first << std::endl;
    }
    
    std::map<int, User *> invited(chan.get_invited());

    for(std::map<int, User *>::iterator it = invited.begin(); it != invited.end(); it++)
    {
        std::cout << "invite: " << it->first << std::endl;
    }
    User    _new_inv(0, "le nouveau", "hostname", "le S", "pas mode");
    chan.add_invited(_new_inv);
    User    autre_new_inv(0, "le nouveau en mieux", "hostname", "Rush B", "rusian mode");
    chan.add_invited(autre_new_inv);
    chan.add_invited(autre_new_inv);
    invited = chan.get_invited();
    for(std::map<int, User *>::iterator it = invited.begin(); it != invited.end(); it++)
    {
        std::cout << "invited: " << it->first << std::endl;
    }
    chan.remove_invited("le nouveau");
    invited = chan.get_invited();
    for(std::map<int, User *>::iterator it = invited.begin(); it != invited.end(); it++)
    {
        std::cout << "invited: " << it->first << std::endl;
    }

    chan.set_mode("i");
    std::cout << "mode: " << chan.get_mode() << std::endl;
    chan.add_user(_new);
    chan.remove_user("le nouveau en mieux");
    chan.add_user(autre_new);
    users = chan.get_users();
    for(std::map<std::string, User>::iterator it = users.begin(); it != users.end(); it++)
    {
        std::cout << "user: " << it->first << std::endl;
    }
    return 1;
}