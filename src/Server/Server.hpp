#ifndef SERVER_HPP
#define SERVER_HPP

#define CONFIG_FILE_PATH "config/default.config"
#define MOTD_FILE_PATH "config/motd.txt"

#include <iostream>
#include <vector>
#include <map>

#include "Channel.hpp"
#include "User.hpp"

class User;
class Channel;

class Server
{
	private:
		//server
		int									listener_fd;
		std::vector<struct pollfd>			pfds;
		std::map<int, User*>				users;
		std::map<std::string, Channel*>		channels;
		std::string							password;
		std::string							uptime;
		std::map<std::string, std::string>	configs;
		
		void								set_uptime();
		void								set_config();
		struct pollfd						new_pfd(int fd, short events, short revents);
		void								get_listener(struct addrinfo *infos);
		void								accept_user();
		
	public:
		// const
		Server(std::string port, std::string pass);
		~Server();

		// access
		std::map<int, User *>				&get_users();
		User								*get_user(std::string nick);
		std::map<std::string, Channel *>	&get_channels();
		Channel								*get_channel(std::string name);
		std::map<std::string, std::string>	&get_configs();
		std::string							get_config(std::string name);
		std::string							get_uptime();
		std::string							get_password();

		// functionalities
		void								run();
		void								add_user(User& user);
		void								delete_user(User &user);
		void								delete_channel(std::string name);
		void								add_channel(Channel new_chan);
		Channel *							add_new_channel(std::string const &name, std::string const &topic, std::string const &mode, User &op);
};

#endif
