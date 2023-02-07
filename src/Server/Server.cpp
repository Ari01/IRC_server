#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>

#include "Server.hpp"

/*****************************************
********** PRIVATE FUNCTIONS *************
*****************************************/

	/* SET UPTIME
	** sets the creation date and time */
	void	Server::set_uptime()
	{
	    std::time_t			t;
		std::tm*			now;
		std::stringstream	ss;
		
		t = std::time(0);
		now = std::localtime(&t);
		ss << (now->tm_year + 1900) << '-';
        ss << (now->tm_mon + 1) << '-';
        ss << now->tm_mday << "\n";
		uptime = ss.str();
	}

	/* SET CONFIG
	** sets name, version from config file and server uptime */
	void	Server::set_config()
	{
		std::ifstream		ifs;
		std::string			line;
		size_t				pos;

		ifs.open(CONFIG_FILE_PATH);
		if (!ifs.good())
		{
			perror("open config file");
			exit(1);
		}
		else
		{
			while (!ifs.eof())
			{
				std::getline(ifs, line);
				if (line.find("#") == std::string::npos)
				{
					pos = line.find("=");
					if (pos != std::string::npos)
						configs[line.substr(0, pos)] = line.substr(pos + 1);
				}
			}
			set_uptime();
		}
	}

	/* NEW_PFD
	** Instanciates a new struct pollfd
	** @param fd : filedescriptor of the pfd
	** @param events : possible events for a pfd are POLLIN (ready to read) and POLLOUT (ready to write)
	** @param revents : events that have occured. Check if revents == POLLIN/POLLOUT 
	   to check if a pfd is ready to read/write
	** @return : the new pfd */
	struct pollfd	Server::new_pfd(int fd, short events, short revents)
	{
		struct pollfd	ret;

		ret.fd = fd;
		ret.events = events;
		ret.revents = revents;
		return (ret);
	}

	/* GET_LISTENER
	** Initializes a socket listener and binds it to host's IP/PORT. This socket will wait (listen)
	   new user connections, which will then be accepted in Server.accept_user()
	   The socket fd is saved in Server.listener_fd
	** @param infos : contains host informations acquired with get_addrinfo() like IP/PORT and
	   socket infos (IPv4, SOCK_STREAM, TCP_PROTOCOL etc) */
	void	Server::get_listener(struct addrinfo *infos)
	{
		struct addrinfo	*ite;
		int				yes;

		listener_fd = -1;
		yes = 1;
		for (ite = infos; ite != NULL; ite = ite->ai_next)
		{
			listener_fd = socket(ite->ai_family, ite->ai_socktype, ite->ai_protocol);
			if (listener_fd == -1)
			{
				perror("server: socket");
				continue;
			}
			if (setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int)) == -1)
			{
				perror("setsockopt");
				exit(1);
			}
			if (bind(listener_fd, ite->ai_addr, ite->ai_addrlen) == -1)
			{
				close(listener_fd);
				perror("server bind");
				continue;
			}
			break;
		}
	}

	/* ACCEPT USER
	** Accepts connections from users, instanciates a new User() object and adds its fd in pfds array */
	void	Server::accept_user()
	{
		int						new_fd;
		socklen_t				addrlen;
		struct sockaddr_in		remote_addr;

		addrlen = sizeof(remote_addr);
		new_fd = accept(listener_fd, (sockaddr *)&remote_addr, &addrlen);
		if (new_fd == -1)
			perror("accept");
		else
		{
			pfds.push_back(new_pfd(new_fd, POLLIN, 0));
			if (fcntl(new_fd, F_SETFL, O_NONBLOCK) == -1)
				perror("fcntl\n");
			else
				users[new_fd] = new User(new_fd, remote_addr);
			std::cout << "new user fd = " << new_fd << std::endl;
		}
	}


/*****************************************
************** CONSTRUCTORS **************
*****************************************/

	/* CONSTRUCTOR
	** Creates a listener socket, adds its fd in pfds array and waits (listens) for user connections */
	Server::Server(std::string port, std::string pass)
	{
		struct addrinfo	hints;
		struct addrinfo	*res;
		int				status;

		password = pass;
		set_config();
		// hints
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		// addinfo
		status = getaddrinfo(NULL, port.c_str(), &hints, &res);
		if (status)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
			exit(1);
		}

		// get listener socket
		get_listener(res);
		freeaddrinfo(res);

		// listen
		if (listener_fd == -1 || listen(listener_fd, 20) == -1)
		{
			perror("listener");
			exit(1);
		}

		// add listener fd in pfds
		pfds.push_back(new_pfd(listener_fd, POLLIN, 0));
		if (fcntl(listener_fd, F_SETFL, O_NONBLOCK))
		{
			perror("fcntl");
			exit(1);
		}
	}

	// DESTRUCTOR
	Server::~Server()
	{
		User									*toDel;
		std::map<int, User *>::iterator			uite;
		std::vector<struct pollfd>::iterator	pite;

		uite = users.begin();
		while (uite != users.end())
		{
			toDel = uite->second;
			uite++;
			delete_user(*toDel);
		}
		for (pite = pfds.begin(); pite != pfds.end(); pite++)
			close(pite->fd);
		pfds.clear();
	}

/*****************************************
*************** ACCESSORS ****************
*****************************************/

	/* GET_PASSWORD */
	std::string	Server::get_password()
	{
		return (password);
	}

	/* GET_USERS
	** @returns the users map */
	std::map<int, User *>		&Server::get_users()
	{
		return (users);
	}

	/* GET_USER
	** Searches for user with nickname in the user map
	** @param nick : the nickname to find
	** @returns : a pointer to the user if a match was found, NULL otherwhise */
	User	*Server::get_user(std::string nick)
	{
		std::map<int, User *>::iterator	ite;

		for (ite = users.begin(); ite != users.end(); ite++)
		{
			if (ite->second->get_nickname() == nick)
				return (ite->second);
		}
		return (NULL);
	}

	/* GET CHANNELS
	** @returns the channels map */
	std::map<std::string, Channel *>	&Server::get_channels()
	{
		return (channels);
	}

	/* GET CHANNEL
	** Searches the channel map for chan name
	** @param name : the name of the chan to find
	** @return : a pointer to the channel if there is a match, NULL otherwhise */
	Channel	*Server::get_channel(std::string name)
	{
		std::map<std::string, Channel *>::iterator	ite;

		ite = channels.find(name);
		if (ite != channels.end())
			return (ite->second);
		return (NULL);
	}

	/* GET CONFIGS
	** @returns the configs map */
	std::map<std::string, std::string>	&Server::get_configs()
	{
		return (configs);
	}

	/* GET UPTIME
	** @returns server creation date */
	std::string	Server::get_uptime()
	{
		return (uptime);
	}

	/* GET CONFIG
	** Searches for key in config map and returns its value if it exists
	** @param key : the name of the config to search for in the map
	** @return : the value of the config */
	std::string	Server::get_config(std::string key)
	{
		if (configs.find(key) != configs.end())
			return (configs[key]);
		std::cerr << "config not found\n";
		return ("");
	}

/*****************************************
************* FUNCTIONALITIES ************
*****************************************/

	/* RUN
	** Main loop : waits for user connections or commands
	** If listener pfd is ready to read (revents == POLLIN occured), then we received a connection
	   request from a client. The connexion is handled in Server.accept_user()
	** If pfd is ready to read but is not the listener's pfd, then we received a message from a client
	   This message is parsed into commands in user.receive_msg
	   The commands are then executed and an appropriate response is sent to the client who sent
	   the commands, in user.send_msg()*/
	void	Server::run()
	{
		int		nfds;

		nfds = pfds.size();
		if (poll(&pfds[0], nfds, -1) == -1)
			perror("poll");
		else
		{
			for (int i = 0; i < nfds; i++)
			{
				if (pfds[i].revents & POLLIN)
				{
					if (pfds[i].fd == listener_fd)
						accept_user();
					else if (!users[pfds[i].fd]->receive_msg(pfds, i, *this))
					{
						delete_user(*users[pfds[i].fd]);
						nfds--;
					}
				}
			}
		}
	}

	/* ADD_USER
	** Adds a user to users map
	** @param user : the user to add */
	void	Server::add_user(User& user)
	{
		user.set_registered(true);
		users[user.get_fd()] = &user;
	}

	/* DELETE_USER
	** Removes a user from the server. To do so, the user is removed from all the channels he was on.
	   His fd is then removed from the pfds table and the user object is deleted from the users map */
	void	Server::delete_user(User &user)
	{
		std::map<std::string, Channel *>::iterator	chan_ite;
		std::vector<struct pollfd>::iterator		pfd_ite;
		std::map<int, User *>::iterator				toDel;
		Channel										*chan;
		int											fd;

		fd = user.get_fd();
		toDel = users.find(fd);
		if (toDel != users.end())
		{
			chan_ite = channels.begin();
			while (chan_ite != channels.end())
			{
				chan = chan_ite->second;
				chan_ite++;
				chan->remove_user(user);
				if (!chan->get_users().size())
					delete_channel(chan->get_name());
			}
			for (pfd_ite = pfds.begin(); pfd_ite != pfds.end(); pfd_ite++)
			{
				if (pfd_ite->fd == fd)
				{
					close(fd);
					pfds.erase(pfd_ite);
					break;
				}
			}
			delete (toDel->second);
			users.erase(fd);
		}
	}

	/* DELETE CHANNEL
	** Removes a channel from the channels map if it exists*/

	/*void	Server::add_channel(Channel new_chan)
	{
		channels.insert(make_pair(new_chan.get_name(), &new_chan));
	}*/

	Channel*	Server::add_new_channel(std::string const &name, std::string const &topic, std::string const &mode, User &op)
	{
		Channel*	chan = new Channel(name, topic, mode, op);

		channels[name] = chan;
		return chan;
	}

	void	Server::delete_channel(std::string name)
	{
		delete channels[name];
		channels.erase(name);
	}
