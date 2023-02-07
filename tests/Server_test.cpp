#include "Server.hpp"

#define HOSTADDR "127.0.0.1"

void	displayChannels(Server &server)
{
	std::map<std::string, Channel *>::iterator	ite;
	std::map<std::string, Channel *>			channels;
	
	channels = server.get_channels();
	std::cout << "Channel list\n";
	std::cout << "----------------\n";
	for (ite = channels.begin(); ite != channels.end(); ite++)
		std::cout << *(ite->second);
}

void	displayUsers(Server &server)
{
	std::map<int, User *>::iterator	ite;
	std::map<int, User *>			users;
	
	users = server.get_users();
	std::cout << "User list\n";
	std::cout << "----------------\n";
	for (ite = users.begin(); ite != users.end(); ite++)
		std::cout << *(ite->second);
}

void	testDeleteUser(Server &server)
{
	std::map<int, User *>		&users = server.get_users();
	std::map<std::string, Channel *>	&channels = server.get_channels();

	// ADDING USERS/CHANNELS
	std::cout << "ADDING USERS AND CHANNELS\n";
	users["Denis"] = new User(0, "Denis", "localhost", "Dchheang", "rwd", "127.0.0.1");
	users["Theo"] = new User(1, "Theo", "localhost", "Theo", "rwt", "127.0.0.1");
	users["Axel"] = new User(2, "Axel", "localhost", "Axel", "rwa", "127.0.0.1");

	channels["Denis chan"] = new Channel("Denis chan", "topic1", "rw", users["Denis"]);
	channels["Theo chan"] = new Channel("Theo chan", "topic2", "rw", users["Theo"]);
	channels["Axel chan"] = new Channel("Axel chan", "topic3", "rw", users["Axel"]);

	// printing users and channels
	displayUsers(server);
	displayChannels(server);

	// deleting users
	std::cout << "DELETING DENIS\n";
	server.delete_user("Denis");
	displayUsers(server);
	displayChannels(server);

	std::cout << "DELETING THEO\n";
	server.delete_user("Theo");
	displayUsers(server);
	displayChannels(server);

	std::cout << "DELETING AXEL\n";
	server.delete_user("Axel");
	displayUsers(server);
	displayChannels(server);
}

int main()
{
	Server	server;

	testDeleteUser(server);
	return (0);
}
