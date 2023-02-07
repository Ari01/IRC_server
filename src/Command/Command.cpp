#include "Command.hpp"
#include "Utils.hpp"

/******************************
********* CONSTRUCTORS ********
******************************/

	/* COMMAND(1)
	** Contructs a command by parsing the message sent by a client
	   The message is split into :
	   . src : the src who sent the msg if there is one specified in the original message
	   . cmd : the command to be executed (/JOIN /PART...)
	   . dsts : the destinators of the message if any
	   . params : the rest of the message which is usually a string to print
	** @param message : the message to parse */
	Command::Command(std::string message)
	{
		std::vector<std::string>	split;

		split = irc::split(message, " ");
		cmd = split[0];
		dsts = "";
		src = "";
		params = "";
		
		for (size_t i = 1; i < split.size(); i++)
		{
			if (params.size())
				params += " ";
			params += split[i];
		}
		std::cout << "my msg is = " << *this << std::endl;
		/*size_t start = 0;
		size_t end = 0;

		if (message[0] == ':')
		{
			start++;
			end = message.find(' ');
			src = message.substr(start, end - start);
			start = end + 1;
		}
		end = message.find(' ', start);
		if (end == std::string::npos)
			cmd = message;
		else
		{
			cmd = message.substr(start, end - start);
			start = end + 1;
			if (message[start] != ':' && message.find(':', start) != std::string::npos)
			{
				end = message.find(" ", start);
				dsts = message.substr(start, end - start);
				//start = end + 1;
			}
			//end = message.find(":", start);
			//params = message.substr(start, end - start);
			params = message.substr(start);
			start = end + 1;
			if (end != std::string::npos)
				params += message.substr(start);
		}
		std::cout << "my msg is = " << *this << std::endl;*/
	}

	/* COMMAND (2)
	** Constructs a command which is a message to be sent to a user
	** @params : see Command(1) split infos*/
	Command::Command(std::string src, std::string cmd, std::string dsts, std::string params)
	{
		if (src.size())
			message = ':' + src + ' ';
		message += cmd + ' ' + dsts + " :" + params + "\r\n";
	}

	Command::~Command()
	{

	}

/******************************
******* SETTERS/GETTERS *******
******************************/
	std::string Command::get_message() const
	{
		return (message);
	}

	std::string	Command::get_src() const
	{
		return (src);
	}

	std::string	Command::get_cmd() const
	{
		return (cmd);
	}

	std::string	Command::get_dsts() const
	{
		return (dsts);
	}

	std::string	Command::get_params() const
	{
		return (params);
	}

	std::string Command::get_reply(std::string code) const
	{
		std::string	reply;

		if (src.size())
			reply = ':' + src + ' ';
		if (code.size())
			reply += code + ' ';
		if (dsts.size())
			reply += dsts + " :";
		return (reply);
	}

	void Command::set_dsts(std::string dst)
	{
		dsts = dst;
	}

	void Command::set_src(std::string src)
	{
		this->src = src;
	}

/******************************
******* FUNCTIONALITES ********
******************************/
	
	/* OPERATOR */
	std::ostream&	operator<<(std::ostream& os, const Command& cmd)
	{
		os << "src = " << cmd.get_src() << std::endl;
		os << "cmd = " << cmd.get_cmd() << std::endl;
		os << "dsts = " << cmd.get_dsts() << std::endl;
		os << "params = " << cmd.get_params() << std::endl;
		os << std::endl;
		return (os);
	}
