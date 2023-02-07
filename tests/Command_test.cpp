#include "Command.hpp"

void	testConstructor(std::string message)
{
	Command	cmd(message);

	std::cout << cmd << std::endl;
}

int main(int ac, char **av)
{
	std::string	message;
	int			i;

	i = 1;
	if (ac <= 1)
		std::cout << "please pass a message command as argument to ./command";
	else
	{
		while (av[i])
			message += av[i++];
		testConstructor(message);
	}
	return (0);
}
