#include "Commands.hpp"
#include <fstream>

void	MOTD(Server &server, User &user, Command &cmd)
{
	std::ifstream	ifs;
	std::string		line;

	ifs.open(MOTD_FILE_PATH);
	if (!ifs.good())
		perror("opening file motd");
	else
	{
		user.send_msg(cmd, "375", server.get_config("name"));
		while (!ifs.eof())
		{
			std::getline(ifs, line);
			user.send_msg(cmd, "372", line);
		}
		user.send_msg(cmd, "376", line);
	}
}
