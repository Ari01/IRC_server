#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include <string>
# include <map>
# include <iostream>
# include "User.hpp"

class User;

    class Channel
    {
        private:
            std::string  _name;
            std::string  _topic;
            std::string  _mode;
            std::string _user_max;
            std::string _pass;
            std::map<int, User *>       _users;
            std::map<int, User *>       _invited;
            std::map<int, std::string>  _users_mode;
            std::vector<std::string>    _banmask;

            bool        _mode_enable(char mode) const;
            bool        _str_is_egal(const char *str, const char *pattern, int m, int n) const;

        public:
            Channel(std::string const name, std::string const topic, std::string const mode, User &op);
            Channel(std::string const &name, std::string const &topic);
            ~Channel();

            //setteur
            void                            set_name(std::string const name);
            void                            set_topic(std::string const topic);
            bool                            set_mode(std::string const mode);
            bool                            unset_mode(std::string const mode);
            void                            set_pass(std::string const pass);
            void                            set_user_max(std::string const user_max);
            bool                            set_user_mode(User user, char const mode);
            bool                            unset_user_mode(User user, char const mode);

            //accesseur
            std::string                     get_name() const;
            std::string                     get_topic() const;
            std::string                     get_mode() const;
            std::string                     get_pass() const;
            std::string                     get_user_max() const;
            std::string                     get_user_mode(User user);
            std::map<int, User *>           get_users() const;
            std::vector<std::string>        get_banmask() const;

            bool                            add_banmask(std::string banmask);
            bool                            remove_banmask(std::string banmask);
            bool                            is_ban(User &user) const;
            bool                            add_user(User &user);
            void                            remove_user(User user);
            std::map<int, User *>           get_invited() const;
            void                            add_invited(User &user);
            void                            remove_invited(User user);
            bool                            is_invited(User user) const;
            void                			broadcast(Command cmd, std::string msg);

			friend std::ostream&	operator<<(std::ostream& os, const Channel& chan);
    };

#endif
