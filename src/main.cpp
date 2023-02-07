#include <cstdlib>
#include <signal.h>
#include "Server.hpp"

bool	end = false;

void	handler(int num)
{
	(void)num;

	std::cout << "sig catched\n" << std::endl;
	end = true;
}

int main(int ac, char **av)
{
	int	port;

	if (ac != 3)
	{
		std::cout << "wrong number of arguments. Correct format is : \n";
		std::cout << "./irc <port> <password>\n";
		return (1);
	}
	else
	{
		port = atoi(av[1]);
		if (port < 1024)
			std::cout << "invalid port. Please enter a port number between 1024 and 65535\n";
		else
		{
			signal(SIGINT, handler);
			Server server(av[1], av[2]);
			while (!end)
				server.run();
		}
	}
	return (0);
}
