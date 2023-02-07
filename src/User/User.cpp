#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "User.hpp"
#include "Command.hpp"
#include "Commands.hpp"

/*************************
****** CONSTRUCTORS ******
*************************/
	
	/* CONST (1) : for debug only, inits all attributes */
	User::User(int fd, std::string nickname, std::string hostname, std::string username, std::string mode, std::string hostaddress)
	{
		this->fd = fd;
		this->nickname = nickname;
		this->hostname = hostname;
		this->username = username;
		this->mode = mode;
		this->hostaddress = hostaddress;
	}

	/* CONST (2) : sets user fd, hostaddress and initializes command map 
	** @param fd : user fd
	** @param hostaddr : user hostadress */
	User::User(int fd, struct sockaddr_in hostaddr) : registered(0)
	{
		this->fd = fd;
		this->hostaddress = inet_ntoa(hostaddr.sin_addr);
		std::cout << "new connection from socket " << hostaddress << std::endl;

		// cmd functions
		cmd_func["NICK"] = &NICK;
		cmd_func["USER"] = &USER;
		cmd_func["PASS"] = &PASS;
		cmd_func["OPER"] = &OPER;
		cmd_func["QUIT"] = &QUIT;
    	cmd_func["JOIN"] = &JOIN;
		cmd_func["PART"] = &PART;
		cmd_func["TOPIC"] = &TOPIC;
		cmd_func["INVITE"] = &INVITE;
    	cmd_func["MODE"] = &MODE;
		cmd_func["PING"] = &PING;
		cmd_func["PRIVMSG"] = &PRIVMSG;
		cmd_func["WHOIS"] = &WHOIS;
		cmd_func["WHO"] = &WHO;
		cmd_func["KICK"] = &KICK;
		cmd_func["NOTICE"] = &NOTICE;


	}

	/* DESTRUCTOR */
	User::~User(){}

/*************************
******** ACCESSORS *******
*************************/

	void User::set_nickname(std::string nickname)
	{
		this->nickname = nickname;
		return;
	}

	void User::set_username(std::string username)
	{
		this->username = username;
		return;
	}

	void User::set_mode(std::string mode)
	{
		this->mode = mode;
		return;
	}

	void User::set_hostname(std::string hostname)
	{
		this->hostname = hostname;
	}

	void User::set_realname(std::string realname)
	{
		this->realname = realname;
	}

	void User::set_password(std::string pass)
	{
		this->password = pass;
	}

	int User::get_fd()
	{
		return(this->fd);
	}

	void User::set_registered(bool b)
	{
		this->registered = b;
	}

	bool User::get_registered()
	{
		return (registered);
	}

	std::string User::get_nickname()
	{
		return(this->nickname);
	}

	std::string User::get_username()
	{
		return(this->username);
	}

	std::string User::get_hostname()
	{
		return (this->hostname);
	}

	std::string User::get_realname()
	{
		return (this->realname);
	}

	std::string User::get_mode()
	{
		return(this->mode);
	}

	std::string User::get_password()
	{
		return(this->password);
	}

	std::string	User::get_prefix()
	{
		std::string	prefix;

		prefix += this->get_nickname() + "!";
		prefix += this->get_username() + "@";
		prefix += this->get_hostname();

		return prefix;
	}

	std::vector<Command *>	&User::get_cmds()
	{
		return (this->commands);
	}

/*******************************
******** FUNCTIONALITIES *******
********************************/

	/* ADD MODE
	** adds a mode to usermodes if it isn't already present
	** @param mode : the mode to add
	** @return : 1 if mode changed, 0 otherwhise */
	int User::add_mode(std::string mode)
	{
		if (this->mode.find(mode) == std::string::npos)
		{
			this->mode += mode;
			return (1);
		}
		return (0);
	}

	/* PARSE CMDS
	** The buff string sent by the client is split into substrings with \r\n as delimiter
	** The appropriate command objects are then created and saved into User.commands map
	** @param buff : the string sent by the client */
	void User::parse_cmds(char *buff)
	{
		std::vector<std::string>	msg_vector;
		std::string					msg;
		size_t						pos;
		
		this->buff += buff;
		pos = this->buff.find("\r\n");
		while (pos != std::string::npos)
		{
			msg = this->buff.substr(0, pos);
			msg_vector.push_back(msg);
			this->buff.erase(0, pos + 2);
			pos = this->buff.find("\r\n");
		}
		for (std::vector<std::string>::iterator ite = msg_vector.begin(); ite != msg_vector.end(); ite++)
		{
			std::cout << "command = " << *ite << std::endl;
			commands.push_back(new Command(*ite));
		}
	}

	/* EXEC CMDS
	** For all cmds in User.commands, do
	** 1) Call the matching command function and execute it
	** 2) Send an appropriate reply msg to the client
	** 3) Erase the pending cmds from User.commands
	** @param server : needed to send infos as replies to clients */
	void User::exec_cmds(Server &server)
	{
		std::vector<Command *>::iterator	ite;
		Command								*cmd;
		std::string							cmd_name;
		std::string							params;
		std::string							tmp;

		for (ite = commands.begin(); ite != commands.end(); ite++)
		{
			cmd = *ite;
			cmd_name = cmd->get_cmd();
			if (cmd_name == "CAP")
				continue ;
			else if (cmd_func.find(cmd_name) == cmd_func.end())
				send_msg(*cmd, "", "Unknown command");
			else
				cmd_func[cmd->get_cmd()](server, *this, *cmd);
			if ((cmd_name == "NICK" || cmd_name == "USER")
				&& nickname.size() && realname.size() && !registered)
			{
				server.add_user(*this);
				cmd->set_dsts(nickname);
				// replies
				send_msg(*cmd, "001", "Welcome to the Internet Relay Network " + get_prefix());
				send_msg(*cmd, "002", "Your host is " + hostname + ", running on version "
					+ server.get_config("version"));
				send_msg(*cmd, "003", "This server was created " + server.get_uptime());
				send_msg(*cmd, "004", server.get_config("name") + " " + server.get_config("version")
					+ " <available modes> <available chanmodes>");
				LUSERS(server, *this, *cmd);
				MOTD(server, *this, *cmd);
			}
		}
		ite = commands.begin();
		while (ite != commands.end())
		{
			cmd = *ite++;
			delete (cmd);
		}
		commands.clear();
	}

	/* RECEIVE MSG
	** recv the string sent by the client into a buffer buff, parses it and execs it (see parse and exec)
	** @param pfds : the array of pfds that are polled
	** @param i : the index of the pfd which we are currently reading on
	** @param server : needed to feed server infos to clients */
	bool	User::receive_msg(std::vector<struct pollfd> pfds, int i, Server &server)
	{
		char				buff[512];
		int					nbytes;
		
		nbytes = recv(pfds[i].fd, buff, sizeof(buff), 0);
		if (nbytes <= 0)
		{
			if (!nbytes)
				std::cout << "pollserver: socket " << pfds[i].fd << " hung up" << std::endl;
			else
				perror("recv");
			return (false);
		}
		else
		{
			buff[nbytes] = 0;
			std::cout << buff << std::endl;
			parse_cmds(buff);
			exec_cmds(server);
		}
		return (true);
	}

	/* SEND MSG
	** Send a reply msg to the client
	** @param cmd : necessary to get the reply msg format (src, cmd or code, dsts)
	** @param code : the reply code if any
	** @param msg : a msg to send, specific to each command situations */
	void User::send_msg(Command const& cmd, std::string code, std::string msg)
	{
		std::string reply;

		reply = cmd.get_reply(code);
		reply += msg + "\r\n";
		std::cout << "reponse = " << reply << std::endl;
		if (send(fd, reply.c_str(), reply.size(), 0) == -1)
			perror("send");
	}

	/* OPERATOR <<
	** For debug only, prints all attribute values in user */
	std::ostream&	operator<<(std::ostream& os, const User& user)
	{
		os << "FD = " << user.fd << std::endl;
		os << "NICK = " << user.nickname << std::endl;
		os << "HOSTNAME = " << user.hostname << std::endl;
		os << "HOSTADRESS = " << user.hostaddress << std::endl;
		os << "USERNAME = " << user.username << std::endl;
		os << "REALNAME = " << user.realname << std::endl;
		os << "MODE = " << user.mode << std::endl << std::endl;
		return (os);
	}
