#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <string>
# include <iostream>
# include <map>
# include "User.hpp"
# include "Command.hpp"
# include "Server.hpp"
# include "Channel.hpp"

class User;
class Server;
class Channel;

namespace irc
{
    std::string                 user_list(User user, Channel &chan, std::map<int, User *> const users);
    std::vector<std::string>    split(std::string str, std::string delimiter);
    std::string                 set_prefix(Command cmd, std::string code);
    std::string                 set_msg(Command cmd, std::string code, std::string msg);
    void                        print_all_chan_user(Server serv); //debug
	std::string					atoi(int n);
    bool                        is_channel_name(std::string name);
    bool                        is_valid_channel_name(std::string const &name);
    int                         find_user(std::map<int, User *> users, std::string nickname);
    std::string                 format_banmask(std::string str);
    std::string                 arg_list(Channel &chan);

	bool isLetter(char c);
	bool isSpecial(char c);
	bool isDigit(char c);
}
#endif