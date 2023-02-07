# include <sstream>
# include "Utils.hpp"

namespace irc
{
    std::string user_list(User user, Channel &chan, std::map<int, User *> users)
    {
        std::string list;

        list += user.get_nickname() + " = ";
        list += chan.get_name() + " :";
        for (std::map<int, User *>::iterator it = users.begin(); it != users.end(); it++)
        {
            if (list.length())
                list += " ";
            //modif verif sur get_user_mode et pas get_mode
            if (chan.get_user_mode(*(it->second)).find("o") != std::string::npos || chan.get_user_mode(*(it->second)).find("O") != std::string::npos)
                list += "@";
            list += it->second->get_nickname();

        }
        return list;
    }

    std::vector<std::string>    split(std::string str, std::string const delimiter)
    {
        std::vector<std::string>    new_vec = std::vector<std::string>();
        size_t                      pos;

        if (str.empty())
        {
            new_vec.push_back("");
            return new_vec;
        }
        if (delimiter.empty())
        {
            new_vec.push_back(str);
            return new_vec;
        }
        while ((pos = str.find(delimiter)) != std::string::npos)
        {
            new_vec.push_back(str.substr(0, pos));
            str.erase(0, pos + delimiter.length());
        }
        new_vec.push_back(str);
        return new_vec;
    }

    std::string set_prefix(Command cmd, std::string code)
    {
        std::string new_str;

        new_str += ":";
        new_str += cmd.get_src();
        new_str += " ";
        new_str += code;
        new_str += " ";
        new_str += cmd.get_dsts();
        new_str += " :";
        return new_str;
    }

    std::string                 set_msg(Command cmd, std::string code, std::string msg)
    {
        std::string new_str;

        new_str += set_prefix(cmd, code);
        new_str += msg;
        return new_str;
    }

	std::string	atoi(int n)
	{
		std::stringstream	ss;

		ss << n;
		return (ss.str());
	}
    
    bool    is_channel_name(std::string name)
    {
        std::string invalid_char = ":, \a";

        if (name[0] != '#')
            return false;
        for (std::string::const_iterator it = name.begin()++; it != name.end(); it++)
        {
            if (invalid_char.find(*it) != std::string::npos)
                return false;
        }
        return true;
    }

   /* bool    is_channel_name(std::string name)
    {
        if (name[0] == '#')
            return true;
        return false;
    }*/

    int find_user(std::map<int, User *> users, std::string nickname)
    {
       for (std::map<int, User *>::iterator it = users.begin(); it != users.end(); it++)
       {
        if (it->second->get_nickname() == nickname)
            return it->second->get_fd();
       }
        return 0; 
    }

    std::string format_banmask(std::string str)
    {
        std::string nick;
        std::string user;
        std::string addr;
        size_t      it;

        if (str.find("!") == std::string::npos && str.find("@") == std::string::npos)
            return str + "!*@*";
        else
        {
            it = str.find("!");
            if (it != std::string::npos && it != 0)
            {
                nick += str.substr(0, it);
                str.erase(0, it + 1);
            }
            else
                nick = "*";

            if (it == 0)
                str.erase(0, it + 1);
            it = str.find("@");
            if (it != 0)
            {
                user = str.substr(0, it);
                if (!user.size())
                    user = "*";
                str.erase(0, it);
            }
            else
                user = "*";
            if (str.size() > 1)
                addr = str;
            else
                addr = "@*";
            return nick + "!" + user + addr;
        }
    }


    std::string arg_list(Channel &chan)
    {
        std::string str = "";
        std::string mode = chan.get_mode();

        for (std::string::iterator it = mode.begin(); it != mode.end(); it++)
        {
            
            if (*it == 'k')
            {
                if (str.length())
                    str += " ";
                str += chan.get_pass();
            }
            if (*it == 'l')
            {
                if (str.length())
                    str += " ";
                str += chan.get_user_max();
            }
        }
        return str;
    }

    bool isLetter(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
    bool isSpecial(char c) { return (c >= '[' && c <= '`') || (c >= '{' && c <= '}'); }
    bool isDigit(char c) { return (c >= '0' && c <= '9'); }
}



