#ifndef USER_HPP
# define USER_HPP

#include <iostream>
#include <string>
#include <vector>

#include "Command.hpp"
#include "Commands.hpp"
#include "Server.hpp"

class Server;

class User
{
    private:
		std::string													buff;
        std::string													nickname;
        std::string													hostname;
        std::string													username;
		std::string													realname;
        std::string													mode;
		std::string													hostaddress;
		std::string													password;
        int															fd;
		bool														registered;
		void														parse_cmds(char *buff);
		void														exec_cmds(Server &server);

    public:
		std::vector<Command *>										commands;
		std::map<std::string, void (*)(Server&, User&, Command&)>	cmd_func;

		User(int fd, std::string nickname, std::string hostname, std::string username, std::string mode);
		User(int fd, std::string nickname, std::string hostname, std::string username, std::string mode, std::string hostaddress);
		User(int fd, struct sockaddr_in hostaddr);
		~User();

		// accessors
		void					set_nickname(std::string nickname);
		void					set_username(std::string username);
		void					set_mode(std::string mode);
		void					set_hostname(std::string hostname);
		void					set_realname(std::string realname);
		void					set_password(std::string pass);
		void					set_registered(bool b);
		bool					get_registered();
		int						get_fd();
		std::string				get_nickname();
		std::string				get_username();
		std::string				get_mode();
		std::string				get_hostname();
		std::string				get_realname();
		std::string				get_prefix();
		std::string				get_password();
		std::vector<Command *>	&get_cmds();

		// func
		int						add_mode(std::string mode);
		bool					receive_msg(std::vector<struct pollfd> pfds, int i, Server &server);
		void					send_msg(Command const& cmd, std::string code, std::string msg);

		friend std::ostream&	operator<<(std::ostream& os, const User& user);
};
#endif
