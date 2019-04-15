#include"Server.h"
#include<iostream>
#include<thread>
bool run = true;

void cmd_thread()
{
	while (true)
	{
		char cmd_buf[128];
		std::cin >> cmd_buf;
		if (strcmp(cmd_buf, "exit") == 0)
		{
			run = false;
			std::cout << "Server exit" << std::endl;
			break;
		}
		else
		{
			std::cout << "Invalid input, please re-enter." << std::endl;
		}
	}
}
int main()
{
	//ÊäÈëÏß³Ì
	std::thread cmd_t(cmd_thread);
	cmd_t.detach();

	Server server;
	server.Bind(IP, PORT);
	server.Listen(5);
	while (run)
	{
		server.OnRun();
	}
	server.CloseServer();
	std::cout << "EXIT...." << std::endl;
	return 0;
}