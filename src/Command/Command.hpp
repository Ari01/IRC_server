#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>

class Command
{
	private:
		std::string	message;
		std::string	src;
		std::string	cmd;
		std::string	dsts;
		std::string	params;

	public:
		Command(std::string message);
		Command(std::string src, std::string cmd, std::string dsts, std::string params);
		~Command();

		std::string					get_src() const;
		std::string					get_cmd() const;
		std::string					get_dsts() const;
		std::string					get_params() const;
		std::string					get_message() const;
		std::string					get_reply(std::string code) const;
		void						set_dsts(std::string dst);
		void						set_src(std::string src);

		friend std::ostream&		operator<<(std::ostream& os, const Command& cmd);
};

#endif
